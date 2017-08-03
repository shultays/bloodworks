#pragma once

#include "cTools.h"
#include "cGlobals.h"
#include "json.h"
#include "DirentHelper.h"

class GameObjectTemplate
{
	friend class BloodworksLuaWorld;
	std::string name;
	std::string scriptFile;
	std::string scriptName;
	sol::table scriptTable;

	sol::table jsonTable;
public:
	GameObjectTemplate(nlohmann::json& j, const DirentHelper::File& file)
	{
		jsonTable = lua.createTableForJson(j);
		name = j["name"].get<std::string>();
		scriptFile = j["scriptFile"].get<std::string>();
		scriptName = j["scriptName"].get<std::string>();

		if (lua[scriptName] == sol::nil)
		{
			lua[scriptName] = lua.create_table();

			std::string scriptPath = file.folder + j["scriptFile"].get<std::string>();
			lua.script_file(scriptPath);
		}
		scriptTable = lua[scriptName];
	}

	const std::string& getName() const
	{
		return name;
	}

	const std::string& getScriptName() const
	{
		return scriptName;
	}
};
