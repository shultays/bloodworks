#pragma once

#include <vector>

#include "cGlobals.h"
#include "json.h"
#include "cTools.h"

class Gun;
class Bullet;

class Perk
{
	std::string name;
	std::string description;
	std::string iconPath;
	sol::table scriptTable;

	int level;
	int maxLevel;

	sol::function onAddGunBulletFunc;
	sol::function onTickFunc;
	sol::function onPlayerDamagedFunc;
public:
	Perk(nlohmann::json& j)
	{
		level = 0;

		name = j["name"].get<std::string>();
		description = j["description"].get<std::string>();
		iconPath = j["iconFile"].get<std::string>();
		scriptTable = lua[j["scriptName"].get<std::string>()] = lua.create_table();

		if (j.count("maxLevel"))
		{
			maxLevel = j["maxLevel"].get<int>();
		}
		else
		{
			maxLevel = 1;
		}

		std::string scriptPath = j["scriptFile"].get<std::string>();
		lua.script_file(scriptPath);

		onAddGunBulletFunc = scriptTable["onAddGunBullet"];
		onPlayerDamagedFunc = scriptTable["onPlayerDamaged"];
		onTickFunc = scriptTable["onTick"];
	}

	void takeLevel();

	int getLevel() const
	{
		return level;
	}

	int getMaxLevel() const
	{
		return maxLevel;
	}

	bool isTakenFully() const
	{
		return maxLevel != -1 && level >= maxLevel;
	}

	const std::string& getIconPath() const
	{
		return iconPath;
	}

	const std::string& getName() const
	{
		return name;
	}

	const std::string& getDescription() const
	{
		return description;
	}

	void onTick();
	void onAddGunBullet(Gun *gun, Bullet *bullet);
	int onPlayerDamaged(int damage, sol::table& params);
};