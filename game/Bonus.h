#pragma once

#include <string>

class Bonus
{
	friend class Bloodworks;
	std::string name;
	std::string scriptName;
	std::string iconPath;
public:
	Bonus(const std::string& bonusDataFile);
};
