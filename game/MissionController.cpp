#include "MissionController.h"
#include "json.h"
#include "cTools.h"
#include "cGlobals.h"

using json = nlohmann::json;

void MissionController::loadMissionController(const std::string& missionControllerData)
{
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

void MissionController::clear()
{
	scriptTable["clear"]();
}

void MissionController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
}

void MissionController::tick(float dt)
{
	scriptTable["onTick"]();
}

