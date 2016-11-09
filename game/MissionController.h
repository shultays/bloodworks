#pragma once

#include <string>

#include "Sol.h"

class Bloodworks;

class MissionController
{
	Bloodworks *bloodworks;

	std::string scriptPath;
	sol::table scriptTable;

	std::string name;

	static int nextGameObjectId;

	std::unordered_map<int, sol::table> gameObjects;
public:
	MissionController(){}
	void loadMissionController(const std::string& missionControllerData);
	void clear();
	void init(Bloodworks *bloodworks);
	void tick(float dt);
	int addGameObject(const std::string& name, const std::string& script, const sol::table& initArgs);
};