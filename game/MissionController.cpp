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


	lua.new_usertype<GameObject>("GameObject",
		"index", sol::readonly(&GameObject::id),

		"toBeRemoved", &GameObject::toBeRemoved,
		"script", &GameObject::script,
		"data", &GameObject::data,
		"addText", &GameObject::addText,
		"addTexture", &GameObject::addTexture,

		"setPosition", &GameObject::setPosition,
		"setRotation", &GameObject::setRotation,
		"setScale", &GameObject::setScale
	);


	lua.new_usertype<GameObject::RenderableData>("RenderableData",
		"position", &GameObject::RenderableData::pos,
		"rotation", &GameObject::RenderableData::rotation,
		"textureSize", &GameObject::RenderableData::textureSize,
		"textSize", &GameObject::RenderableData::textSize,
		"color", &GameObject::RenderableData::color,
		"update", &GameObject::RenderableData::update,
		"alignment", &GameObject::RenderableData::alignment,
		"textAlignment", &GameObject::RenderableData::textAlignment
		);
}

void MissionController::clear()
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

void MissionController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	lua.set_function("addGameObject",
		[&](const std::string& script) -> GameObject*
	{
		return addGameObject(script);
	});

	lua.set_function("addCustomBullet",
		[&](const sol::table& params) -> Bullet*
	{
		return addCustomBullet(params);
	});
}

void MissionController::tick(float dt)
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

Bullet* MissionController::addCustomBullet(const sol::table& params)
{
	Vec2 dir = bloodworks->getPlayer()->getAimDir();
	Bullet *bullet = new Bullet(bloodworks, nullptr);
	Player *player = bloodworks->getPlayer();

	bullet->pos = player->getPos() + player->getAimDir() * 20;
	bullet->moveSpeed = 20.0f;
	bullet->moveAngle = player->getAimDir().toAngle();
	bullet->radius = 2.0f;
	bullet->damage = 10;
	bloodworks->getBulletController()->addBullet(bullet);

	return bullet;
}
