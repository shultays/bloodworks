#pragma once

#include <string>
#include "cTools.h"
#include "json.h"
#include "sol.h"
#include "DirentHelper.h"

class Bloodworks;

class Bonus
{
	std::string name;
	std::string scriptName;
	std::string iconPath;

	float spawnChance;
	sol::function dynamicSpawnChance;
	Bloodworks *bloodworks;
public:
	Bonus(Bloodworks *bloodworks, nlohmann::json& j, const DirentHelper::File& file);
	void spawnAt(const Vec2& pos);
	const std::string& getIconPath() const
	{
		return iconPath;
	}
	const std::string& getName() const
	{
		return name;
	}
	const std::string& getScriptName() const
	{
		return scriptName;
	}
	void reset();
	float getSpawnChance();
};
