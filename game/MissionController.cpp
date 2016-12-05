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

using json = nlohmann::json;

void MissionController::loadMissionController(const std::string& missionControllerData)
{
	lua["gameObjects"] = lua.create_table();

	this->bloodworks = bloodworks;
	std::string jsonFile;
	textFileRead(missionControllerData.c_str(), jsonFile);
	json j = json::parse(jsonFile.c_str());

	name = j["name"].get<std::string>();

	scriptTable = lua[j["scriptName"].get<std::string>()] = lua.create_table();
	scriptPath = j["scriptFile"].get<std::string>();
	lua.script_file(scriptPath);
	scriptTable["init"]();
}

MissionController::~MissionController()
{
	scriptTable["clear"]();


	for (auto& g : gameObjects)
	{
		auto& gameObject = g.second;

		SAFE_DELETE(gameObject);
	}
	gameObjects.clear();

	lua["gameObjects"] = lua.create_table();
}

MissionController::MissionController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
}

void MissionController::tick()
{
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
