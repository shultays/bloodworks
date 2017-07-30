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
}

void MissionController::tick()
{
	ADD_SCOPED_TIME_PROFILER("MissionController::tick");
	if (loadedMission == -1)
	{
		return;
	}
	gameSpeedMultiplier.tick();
	lua["missionTime"] = timer.getTime() - missionLoadTime;
	lua["missionLoadTime"] = missionLoadTime;
	scriptTable["onTick"]();

	std::vector<int> toBeRemoved;
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

GameObject* MissionController::addGameObject(const std::string& script)
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

		if (lua[script]["init"])
		{
			lua[script]["init"](object);
		}
	}

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
	MissionData data;
	data.name = j["name"].get<std::string>();
	data.scriptName = j["scriptName"].get<std::string>();
	data.scriptFile = file.folder + j["scriptFile"].get<std::string>();
	missions.push_back(data);
}

void MissionController::loadMission(const std::string& name)
{
	loadedMission = -1;
	for (int i = 0; i<missions.size(); i++)
	{
		auto& mission = missions[i];
		if (mission.name == name)
		{
			loadedMission = i;
			scriptTable = lua[mission.scriptName] = lua.create_table();
			missionLoadTime = timer.getTime();

			lua["missionTime"] = timer.getTime() - missionLoadTime;
			lua["missionLoadTime"] = missionLoadTime;

			lua.script_file(mission.scriptFile);
			scriptTable["init"]();
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
		scriptTable = lua[missions[loadedMission].scriptName] = lua.create_table();
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
		if (lua[gameObject->script]["repositionGUI"])
		{
			lua[gameObject->script]["repositionGUI"](gameObject);
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

