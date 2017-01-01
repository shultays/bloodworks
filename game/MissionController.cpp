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

MissionController::~MissionController()
{
	reset();
}

MissionController::MissionController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	loadedMission = -1;
	missionLoop.setSample(resources.getSoundSample("resources/sounds/loop2.ogg"));
	missionLoop.setLooped(true);
	missionLoop.setVolume(0.3f);
	missionLoop.setSpeed(1.2f);
	soundSpeed = 1.0f;
	musicVolume = 1.0f;
	reset();
}

void MissionController::tick()
{
	if (loadedMission == -1)
	{
		return;
	}
	lua["missionTime"] = timer.getTime() - missionLoadTime;
	lua["missionLoadTime"] = missionLoadTime;
	scriptTable["onTick"]();

	std::vector<int> toBeRemoved;
	for (auto& g : gameObjects)
	{
		auto& gameObject = g.second;
		if (gameObject->hasOnTick)
		{
			lua[gameObject->script]["onTick"](gameObject);
			if (gameObject->toBeRemoved)
			{
				toBeRemoved.push_back(g.first);
			}
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
			missionLoopHandle.setVolume(musicVolume * 0.3f);
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
			missionLoopHandle.setVolume(musicVolume * 0.3f);
		}
	}

	if (abs(soundSpeed - bloodworks->getSoundSpeed()) > 0.05f)
	{
		soundSpeed = bloodworks->getSoundSpeed();
		missionLoopHandle.setSpeed(1.2f * (soundSpeed * 0.15f + 0.85f));
	}
}

GameObject* MissionController::addGameObject(const std::string& script)
{
	GameObject *object = new GameObject(bloodworks);
	sol::table gameObject = lua["gameObjects"][object->id] = lua.create_table();

	object->data = gameObject;
	object->hasOnTick = lua[script]["onTick"];
	object->hasBulletCollision = lua[script]["onBulletCollision"];
	object->hasBulletCollision = lua[script]["onPlayerCollision"];
	object->hasBulletCollision = lua[script]["onMonsterCollision"];
	object->script = script;
	gameObjects[object->id] = object;

	lua[script]["init"](object);
	return object;
}

void MissionController::removeGameObject(int id)
{
	lua["gameObjects"][id] = nullptr;
	auto& gameObject = gameObjects[id];
	SAFE_DELETE(gameObject);
	gameObjects.erase(id);
}

void MissionController::addMission(nlohmann::json &j)
{
	MissionData data;
	data.name = j["name"].get<std::string>();
	data.scriptName = j["scriptName"].get<std::string>();
	data.scriptFile = j["scriptFile"].get<std::string>();
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

			lua["gameObjects"] = lua.create_table();
			lua["mission"] = lua.create_table();

			lua.script_file(mission.scriptFile);
			scriptTable["init"]();
			missionLoopHandle = missionLoop.play();
			return;
		}
	}
}

void MissionController::reset()
{
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

