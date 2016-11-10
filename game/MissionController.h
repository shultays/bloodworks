#pragma once

#include <string>

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"

class Bloodworks;
class cRenderable;
class Bullet;
class GameObject;


class MissionController
{
	Bloodworks *bloodworks;

	std::string scriptPath;
	sol::table scriptTable;

	std::string name;
	
	std::unordered_map<int, GameObject*> gameObjects;
public:
	MissionController(){}
	void loadMissionController(const std::string& missionControllerData);
	void clear();
	void init(Bloodworks *bloodworks);
	void tick(float dt);
	GameObject* addGameObject(const std::string& script);
	void removeGameObject(int id);
	Bullet* addCustomBullet(const sol::table& params);
};