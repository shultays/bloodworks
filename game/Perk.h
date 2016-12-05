#pragma once

#include <vector>

#include "cGlobals.h"
#include "json.h"
#include "cTools.h"

using json = nlohmann::json;
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
public:
	void load(const std::string& perkData)
	{
		level = 0;

		std::string jsonFile;
		textFileRead(perkData.c_str(), jsonFile);
		json j = json::parse(jsonFile.c_str());

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
		return level >= maxLevel;
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
};