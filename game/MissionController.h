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
public:
	MissionController(){}
	void loadMissionController(const std::string& missionControllerData);
	void clear();
	void init(Bloodworks *bloodworks);
	void tick(float dt);

};