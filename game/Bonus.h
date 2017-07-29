#pragma once

#include <string>
#include "cTools.h"
#include "json.h"
#include "sol.h"
#include "DirentHelper.h"

class Bloodworks;
class Gun;
class Bullet;
class Monster;

class Bonus
{
	friend class BloodworksLuaWorld;
	std::string name;
	std::string scriptName;
	std::string iconPath;

	float spawnChance;
	Bloodworks *bloodworks;

	bool active;
	sol::table data;

	sol::function spawnFunc;

	sol::function dynamicSpawnChance;
	sol::function onAddGunBulletFunc;
	sol::function onTickFunc;
	sol::function onPlayerDamagedFunc;
	sol::function onMonsterDiedFunc;
	sol::function onPlayerDiedFunc;
	sol::function onReloadFunc;
	sol::function onPlayerPickedGunFunc;
	sol::function onPlayerPickedBonusFunc;
	sol::function onMonsterDamagedFunc;
public:
	Bonus(Bloodworks *bloodworks, nlohmann::json& j, const DirentHelper::File& file);
	void spawnAt(const Vec2& pos);
	const std::string& getIconPath() const
	{
		return iconPath;
	}
	const std::string& getName() const
	{
		return name;
	}
	const std::string& getScriptName() const
	{
		return scriptName;
	}
	void reset();
	float getSpawnChance();

	bool isActive() const
	{
		return active;
	}

	void onTick();
	void onAddGunBullet(Gun *gun, Bullet *bullet);
	void onReload(Gun *gun);
	int onPlayerDamaged(int damage, float dir, sol::table& params);
	void onPlayerDied();
	void onMonsterDied(Monster* monster);
	void setActive(bool active);
	void onPlayerPickedGun(Gun *gun);
	void onPlayerPickedBonus(Bonus *bonus, const Vec2& pos);
	void onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args);
};
