#include "MissionController.h"
#include "json.h"
#include "cTools.h"
#include "cGlobals.h"
#include "cRenderable.h"
#include "cTexture.h"
#include "cFont.h"
#include "Bloodworks.h"
#include "Player.h"
#include "GameObject.h"
#include "BulletController.h"
#include "Bullet.h"
#include "Monster.h"
#include "BloodworksConfig.h"
#include "cTimeProfiler.h"
#include "GameObjectTemplate.h"
#include "BloodworksSteam.h"

MissionController::MissionController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	loadedMission = -1;
	missionLoop.setSample(resources.getSoundSample("resources/sounds/game_music.ogg"));
	missionLoop.setLooped(true);
	missionLoop.setSpeed(1.0f);
	missionLoop.setVolume(bloodworks->getConfig()->getMusicVolume());
	soundSpeed = 1.0f;
	musicVolume = 1.0f;
	gameSpeedMultiplier.setBaseValue(1.0f);
	reset();
}

MissionController::~MissionController()
{
	reset();
	for (auto mod : missionMods)
	{
		SAFE_DELETE(mod);
	}
	missionMods.clear();

	for (auto mission : missions)
	{
		SAFE_DELETE(mission);
	}
	missions.clear();
}

void MissionController::tick()
{
	LastEntrySet S("MissionController::tick");
	if (bloodworks->isFirstTick())
	{
		out << "MissionController::tick\n";
	}
	ADD_SCOPED_TIME_PROFILER("MissionController::tick");
	if (loadedMission == -1)
	{
		return;
	}
	gameSpeedMultiplier.tick();
	lua["missionTime"] = timer.getTime() - missionLoadTime;
	lua["missionLoadTime"] = missionLoadTime;
	scriptTable["onTick"]();

	missions[loadedMission]->persistent.check();

	if (loadedMission != -1)
	{
		for (auto mod : missionMods)
		{
			mod->onTick();
		}
	}

	cVector<int> toBeRemoved;
	for (auto& g : gameObjects)
	{
		auto& gameObject = g.second;
		if (gameObject->onTick)
		{
			gameObject->onTick(gameObject);
		}

		if (gameObject->toBeRemoved)
		{
			toBeRemoved.push_back(g.first);
		}
	}

	for (auto& toRemove : toBeRemoved)
	{
		removeGameObject(toRemove);
	}

	if (bloodworks->isPaused())
	{
		if (musicVolume > 0.0f)
		{
			musicVolume -= timer.getNonSlowedDt() * 4.0f;
			if (musicVolume < 0.0f)
			{
				musicVolume = 0.0f;
			}
			updateMusicVolume();
		}
	}
	else
	{
		if (musicVolume < 1.0f)
		{
			musicVolume += timer.getNonSlowedDt() * 4.0f;
			if (musicVolume > 1.0f)
			{
				musicVolume = 1.0f;
			}
			updateMusicVolume();
		}
	}

	if (abs(soundSpeed - bloodworks->getSoundSpeed()) > 0.05f)
	{
		soundSpeed = bloodworks->getSoundSpeed();
		if (soundSpeed > 0.99f)
		{
			missionLoopHandle.setSpeed(1.0f);
		}
		else
		{
			missionLoopHandle.setSpeed(1.0f * (soundSpeed * 0.08f + 0.92f));
		}
	}
}

GameObject* MissionController::addGameObject(const std::string& script, GameObjectTemplate *gameObjectTemplate, const sol::table& params)
{
	GameObject *object = new GameObject(bloodworks);
	sol::table gameObject = lua["gameObjects"][object->id] = lua.create_table();

	object->data = gameObject;
	object->script = script;

	gameObjects[object->id] = object;

	if (script.length() > 0)
	{
		object->onTick = lua[script]["onTick"];
		object->onBulletCollision = lua[script]["onBulletCollision"];
		object->onPlayerCollision = lua[script]["onPlayerCollision"];
		object->onMonsterCollision = lua[script]["onMonsterCollision"];
		object->onReposition = lua[script]["repositionGUI"];

		if (lua[script]["init"])
		{
			lua[script]["init"](object, params, gameObjectTemplate);
		}
	}

	return object;
}

GameObject* MissionController::addGameObjectUsingTemplate(const std::string& templateName, const sol::table& params)
{
	GameObjectTemplate *gameObjectTemplate = bloodworks->getGameObjectTemplate(templateName);
	GameObject* object = addGameObject(gameObjectTemplate->getScriptName(), gameObjectTemplate, params);
	return object;
}

void MissionController::removeGameObject(int id)
{
	lua["gameObjects"][id] = nullptr;
	auto& gameObject = gameObjects[id];
	SAFE_DELETE(gameObject);
	gameObjects.erase(id);
}

void MissionController::addMission(nlohmann::json& j, const DirentHelper::File& file)
{
	MissionData* data = new MissionData();
	data->basePath = file.folder;
	if (j.count("name"))
	{
		data->name = j["name"].get<std::string>();
	}
	if (j.count("icon") && j["icon"].get<std::string>().size())
	{
		data->icon = file.folder + j["icon"].get<std::string>();
		fixFilePath(data->icon);
	}
	if (j.count("description"))
	{
		data->description = j["description"].get<std::string>();
	}

	data->scriptName = j["scriptName"].get<std::string>();
	data->scriptFile = file.folder + j["scriptFile"].get<std::string>();
	fixFilePath(data->scriptFile);


	data->persistent.setFileBackup("mod_configs/" + data->scriptName + ".txt");

	missions.push_back(data);
}

void MissionController::addMissionMod(nlohmann::json& j, const DirentHelper::File& file)
{
	missionMods.push_back(new MissionMod(bloodworks, j, file));
}

void MissionController::loadMission(const std::string& name)
{
	loadedMission = -1;
	for (int i = 0; i<missions.size(); i++)
	{
		auto& mission = missions[i];
		if (mission->scriptName == name)
		{
			loadedMission = i;
			scriptTable = lua[mission->scriptName] = lua.create_table();
			missionLoadTime = timer.getTime();

			lua["missionTime"] = timer.getTime() - missionLoadTime;
			lua["missionLoadTime"] = missionLoadTime;
			lua["missionScript"] = mission->scriptName;
			lua["missionPath"] = mission->basePath;
			lua["missionConfig"] = mission->persistent;

			bloodworks->loadLuaFile(mission->scriptFile);
			scriptTable["init"]();

			for (auto mod : missionMods)
			{
				mod->init();
			}

			missionLoopHandle = missionLoop.play();
			updateMusicVolume();
			return;
		}
	}
}

void MissionController::reset()
{
	lua["gameObjects"] = lua.create_table();
	lua["mission"] = lua.create_table();
	gameSpeedMultiplier.clear();
	if (loadedMission >= 0)
	{
		if ((bool)getMissionData()["isSurvival"])
		{
			float time = bloodworks->getPlayer()->getDieTime() - missionLoadTime;
			int score = bloodworks->getPlayer()->getScore();
			bloodworks->getSteam()->uploadLeaderboards(score, time);
		}

		scriptTable = lua[missions[loadedMission]->scriptName] = lua.create_table();
		loadedMission = -1;
	}
	for (auto& g : gameObjects)
	{
		auto& gameObject = g.second;

		SAFE_DELETE(gameObject);
	}
	gameObjects.clear();
	
	if (missionLoopHandle.isValid())
	{
		missionLoopHandle.stop();
	}
	soundSpeed = 1.0f;
	musicVolume = 1.0f;
}

void MissionController::onPlayerDied()
{
	if (scriptTable["onPlayerDied"])
	{
		scriptTable["onPlayerDied"]();
	}
}

bool MissionController::isLoaded() const
{
	return loadedMission >= 0;
}

sol::table MissionController::getMissionData()
{
	return lua["missionData"];
}

void MissionController::onMonsterDied(Monster* monster)
{
	if (scriptTable["onMonsterDied"])
	{
		scriptTable["onMonsterDied"](monster);
	}
}

void MissionController::repositionGUI()
{
	for (auto& g : gameObjects)
	{
		auto& gameObject = g.second;

		if (gameObject->onReposition )
		{
			gameObject->onReposition(gameObject, bloodworks->getScreenDimensions().toVec());
		}
	}
}

void MissionController::updateMusicVolume()
{
	if (missionLoopHandle.isValid())
	{
		missionLoopHandle.setVolume(musicVolume * bloodworks->getConfig()->getMusicVolume());
	}
}

void MissionController::onDebugTick()
{
	if (loadedMission != -1)
	{
		if (scriptTable["onDebugTick"])
		{
			scriptTable["onDebugTick"]();
		}
	}
}

bool MissionController::canExit()
{
	if (loadedMission != -1)
	{
		if (scriptTable["canExit"])
		{
			return scriptTable["canExit"]();
		}
	}

	return true;
}

std::string MissionController::getCurrentMissionScript()
{
	if (loadedMission != -1)
	{
		return missions[loadedMission]->scriptName;
	}
	return "";
}

MissionMod::MissionMod(Bloodworks* bloodworks, nlohmann::json& j, const DirentHelper::File& file)
{
	std::string scriptName = j["scriptName"].get<std::string>();
	std::string scriptFile = file.folder + j["scriptFile"].get<std::string>();

	lua[scriptName] = lua.create_table();

	fixFilePath(scriptFile);
	bloodworks->loadLuaFile(scriptFile);

	sol::table table = lua[scriptName];

	initFunc = table["init"];
	onTickFunc = table["onTick"];

	persistent.setFileBackup("mod_configs/" + scriptName + ".txt");
}

MissionMod::~MissionMod()
{
	persistent.check();
}

void MissionMod::init()
{
	data = lua.create_table();
	if (initFunc)
	{
		initFunc(this);
	}
}

void MissionMod::onTick()
{
	if (onTickFunc)
	{
		onTickFunc(this);
	}

	persistent.check();
}
