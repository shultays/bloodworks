#pragma once

#include "GameObjectTemplate.h"
#include "Bloodworks.h"

GameObjectTemplate::GameObjectTemplate(Bloodworks* bloodworks, nlohmann::json& j, const DirentHelper::File& file)
{
	jsonTable = lua.createTableForJson(j);
	name = j["name"].get<std::string>();
	scriptFile = j["scriptFile"].get<std::string>();
	scriptName = j["scriptName"].get<std::string>();

	basePath = file.folder;

	if (lua[scriptName] == sol::nil)
	{
		scriptTable = lua[scriptName] = lua.create_table();

		std::string scriptPath = file.folder + j["scriptFile"].get<std::string>();
		bloodworks->loadLuaFile(scriptPath);
	}
	scriptTable = lua[scriptName];
	if (scriptTable["onLoad"])
	{
		scriptTable["onLoad"](this);
	}
}
