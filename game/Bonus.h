#pragma once

#include <string>
#include "cGlobals.h"

class Bonus
{
	std::string name;
	std::string scriptName;
	std::string iconPath;
public:
	Bonus(const std::string& bonusDataFile);
	void spawnAt(const Vec2& pos);
	const std::string& getIconPath() const;
	const std::string& getName() const;
};
