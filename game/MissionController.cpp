#include "MissionController.h"
#include "json.h"
#include "cTools.h"
#include "cGlobals.h"

using json = nlohmann::json;

int MissionController::nextGameObjectId = 0;

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

void MissionController::clear()
{
	scriptTable["clear"]();
}

void MissionController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	lua.set_function("addGameObject",
		[&](std::string name, std::string script, sol::table initArgs) -> int
	{
		return addGameObject(name, script, initArgs);
	});
}

void MissionController::tick(float dt)
{
	scriptTable["onTick"]();
}

int MissionController::addGameObject(const std::string& name, const std::string& script, const sol::table& initArgs)
{
	int id = nextGameObjectId++;
	sol::table gameObject = lua["gameObjects"][id] = lua.create_table();
	gameObject["name"] = name;
	gameObject["initArgs"] = initArgs;
	gameObject["script"] = script;
	gameObject["id"] = id;

	lua[script]["init"](id, name, initArgs);

	return id;
}

