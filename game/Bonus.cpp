#include "Bonus.h"

#include "cTools.h"
#include "cGlobals.h"
#include "json.h"

Bonus::Bonus(Bloodworks *bloodworks, nlohmann::json& j, const DirentHelper::File& file)
{
	this->bloodworks = bloodworks;

	name = j["name"].get<std::string>();
	iconPath = file.folder + j["iconFile"].get<std::string>();
	scriptName = j["scriptName"].get<std::string>();
	std::string scriptFile = file.folder + j["scriptFile"].get<std::string>();

	spawnChance = 1.0f;
	if (j.count("spawnChance"))
	{
		spawnChance = j["spawnChance"].get<float>();
	}
	lua[scriptName] = lua.create_table();
	std::string folder = file.folder;
	fixFolderPath(folder);
	lua[scriptName]["basePath"] = folder;
	lua.script_file(scriptFile);
	dynamicSpawnChance = lua[scriptName]["variableBonusChance"];
}

void Bonus::spawnAt(const Vec2& pos)
{
	lua[scriptName]["spawn"](pos);
}

void Bonus::reset()
{
	if (lua[scriptName]["clear"])
	{
		lua[scriptName]["clear"]();
	}
	lua[scriptName]["data"] = lua.create_table();
}

float Bonus::getSpawnChance()
{
	if (dynamicSpawnChance)
	{
		return dynamicSpawnChance();
	}
	return spawnChance;
}
