#pragma once

#include "cMat.h"
#include "cVec.h"

#include "Sol.h"
#include "json.h"

class cLuaWorld : public sol::state
{
	void fillTableWithJson(nlohmann::json& j, sol::table& table);
public:
	sol::table createTableForJson(nlohmann::json& j);
};