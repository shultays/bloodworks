#pragma once

#include "cTools.h"
#include "cGlobals.h"
#include "json.h"
#include "DirentHelper.h"

class Bloodworks;

class GameObjectTemplate
{
	friend class BloodworksLuaWorld;
	std::string name;
	std::string scriptFile;
	std::string scriptName;
	std::string basePath;
	sol::table scriptTable;

	sol::table jsonTable;
public:
	GameObjectTemplate(Bloodworks* bloodworks, nlohmann::json& j, const DirentHelper::File& file);

	const std::string& getName() const
	{
		return name;
	}

	const std::string& getScriptName() const
	{
		return scriptName;
	}
};
