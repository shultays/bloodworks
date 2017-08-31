#pragma once

#include <vector>

#include "json.h"
#include "sol.h"
#include "DirentHelper.h"

class Bloodworks;
class Bullet;
class Monster;
class Gun;
class Bonus;

class Perk
{
	friend class BloodworksLuaWorld;

	std::string name;
	std::string path;
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
	sol::function onPlayerDiedFunc;
	sol::function onPlayerPickedGunFunc;
	sol::function onPlayerPickedBonusFunc;
	sol::function onMonsterDamagedFunc;

	Bloodworks* bloodworks;
public:
	Perk(Bloodworks *bloodworks, nlohmann::json& j, const DirentHelper::File& file);

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

	const std::string& getScriptName() const
	{
		return scriptName;
	}

	void onTick();
	void onAddGunBullet(Gun *gun, Bullet *bullet);
	void onReload(Gun *gun);
	int onPlayerDamaged(int damage, float dir, sol::table& params);
	void onPlayerDied();
	void onMonsterDied(Monster *monster);
	void onPlayerPickedGun(Gun *gun);
	void onPlayerPickedBonus(Bonus *bonus, const Vec2& pos);

	void reset();

	bool isLevelVisible() const
	{
		return hideLevelText == false && maxLevel > 1;
	}
	int onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args);
};