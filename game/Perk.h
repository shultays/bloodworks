#pragma once

#include <vector>

#include "cGlobals.h"
#include "json.h"
#include "cTools.h"

using json = nlohmann::json;

class Perk
{
	friend class Bloodworks;
	std::string name;
	std::string description;
	std::string iconPath;
	sol::table scriptTable;

	int level;
	int maxLevel;

	sol::function onAddGunBullet;
	sol::function onTick;
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

		onAddGunBullet = scriptTable["onAddGunBullet"];
		onTick = scriptTable["onTick"];
	}

	void takeLevel()
	{
		if (level < maxLevel)
		{
			level++;
			if (maxLevel == 1)
			{
				scriptTable["init"]();
			}
			else
			{
				scriptTable["init"](level);
			}
		}
	}

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
};