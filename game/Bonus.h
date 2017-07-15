#pragma once

#include <string>
#include "cTools.h"
#include "json.h"

class Bonus
{
	std::string name;
	std::string scriptName;
	std::string iconPath;
public:
	Bonus(nlohmann::json& j);
	void spawnAt(const Vec2& pos);
	const std::string& getIconPath() const;
	const std::string& getName() const;
	void reset();
};
