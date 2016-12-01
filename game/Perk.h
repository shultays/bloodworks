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
	sol::table scriptTable;
	bool used;

	sol::function onAddGunBullet;
public:
	void load(const std::string& perkData)
	{
		used = false;

		std::string jsonFile;
		textFileRead(perkData.c_str(), jsonFile);
		json j = json::parse(jsonFile.c_str());

		name = j["name"].get<std::string>();
		scriptTable = lua[j["scriptName"].get<std::string>()] = lua.create_table();

		std::string scriptPath = j["scriptFile"].get<std::string>();
		lua.script_file(scriptPath);

		onAddGunBullet = scriptTable["onAddGunBullet"];
	}

	void use()
	{
		if (used == false)
		{
			used = true;
			scriptTable["init"]();
		}
	}
};