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
	std::string scriptName;
	sol::table scriptTable;

	int level;
	int maxLevel;
	bool hideLevelText;

	sol::function onAddGunBulletFunc;
	sol::function onTickFunc;
	sol::function onPlayerDamagedFunc;
	sol::function onReloadFunc;
public:
	Perk(nlohmann::json& j)
	{
		name = j["name"].get<std::string>();
		description = j["description"].get<std::string>();
		iconPath = j["iconFile"].get<std::string>();
		scriptName = j["scriptName"].get<std::string>();

		if (j.count("maxLevel"))
		{
			maxLevel = j["maxLevel"].get<int>();
		}
		else
		{
			maxLevel = 1;
		}

		hideLevelText = j.count("hideLevelText") > 0;

		level = 0;
		scriptTable = lua[scriptName] = lua.create_table();

		std::string scriptPath = j["scriptFile"].get<std::string>();
		lua.script_file(scriptPath);

		onAddGunBulletFunc = scriptTable["onAddGunBullet"];
		onPlayerDamagedFunc = scriptTable["onPlayerDamaged"];
		onTickFunc = scriptTable["onTick"];
		onReloadFunc = scriptTable["onReload"];

		reset();
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
	void onReload(Gun *gun);
	int onPlayerDamaged(int damage, sol::table& params);
	void reset();
	void onPlayerDied();

	bool isLevelVisible() const
	{
		return hideLevelText == false && maxLevel > 1;
	}
};