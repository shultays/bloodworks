#include "Bonus.h"

#include "cTools.h"
#include "cGlobals.h"
#include "json.h"

Bonus::Bonus(nlohmann::json& j)
{
	name = j["name"].get<std::string>();
	iconPath = j["iconFile"].get<std::string>();
	scriptName = j["scriptName"].get<std::string>();
	std::string scriptFile = j["scriptFile"].get<std::string>();

	spawnChance = 1.0f;
	if (j.count("spawnChance"))
	{
		spawnChance = j["spawnChance"].get<float>();
	}
	lua[scriptName] = lua.create_table();
	lua.script_file(scriptFile);
	dynamicSpawnChance = lua[scriptName]["variableBonusChance"];
}

void Bonus::spawnAt(const Vec2& pos)
{
	lua[scriptName]["spawn"](pos);
}

const std::string& Bonus::getIconPath() const
{
	return iconPath;
}

const std::string& Bonus::getName() const
{
	return name;
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
