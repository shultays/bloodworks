#pragma once

#include <vector>

#include "json.h"
#include "sol.h"

class Gun;
class Bullet;
class Monster;

class Perk
{
	std::string name;
	std::string description;
	std::string iconPath;
	std::string scriptName;
	sol::table scriptTable;

	int level;
	int maxLevel;
	bool hideLevelText;

	sol::function onAddGunBulletFunc;
	sol::function onTickFunc;
	sol::function onPlayerDamagedFunc;
	sol::function onMonsterDiedFunc;
	sol::function onReloadFunc;
public:
	Perk(nlohmann::json& j);

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
		return maxLevel != -1 && level >= maxLevel;
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
	void onReload(Gun *gun);
	int onPlayerDamaged(int damage, float dir, sol::table& params);
	void reset();
	void onPlayerDied();

	bool isLevelVisible() const
	{
		return hideLevelText == false && maxLevel > 1;
	}
	void onMonsterDied(Monster* monster);
};