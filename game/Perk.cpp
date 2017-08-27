#include "Perk.h"
#include "Gun.h"
#include "Monster.h"
#include "Bullet.h"
#include "Bonus.h"
#include "cGlobals.h"

Perk::Perk(Bloodworks *bloodworks, nlohmann::json& j, const DirentHelper::File& file)
{
	this->bloodworks = bloodworks;
	name = j["name"].get<std::string>();
	description = j["description"].get<std::string>();
	iconPath = file.folder + j["iconFile"].get<std::string>();
	scriptName = j["scriptName"].get<std::string>();

	if (j.count("maxLevel"))
	{
		maxLevel = j["maxLevel"].get<int>();
	}
	else
	{
		maxLevel = 1;
	}

	hideLevelText = j.count("hideLevelText") > 0;

	level = 0;
	scriptTable = lua[scriptName] = lua.create_table();
	std::string folder = file.folder;
	fixFolderPath(folder);
	scriptTable["basePath"] = folder;

	std::string scriptPath = file.folder + j["scriptFile"].get<std::string>();
	lua.script_file(scriptPath);

	onAddGunBulletFunc = scriptTable["onAddGunBullet"];
	onPlayerDamagedFunc = scriptTable["onPlayerDamaged"];
	onTickFunc = scriptTable["onTick"];
	onReloadFunc = scriptTable["onReload"];
	onMonsterDiedFunc = scriptTable["onMonsterDied"];
	onPlayerDiedFunc = scriptTable["onPlayerDied"];
	onPlayerPickedGunFunc = scriptTable["onPlayerPickedGun"];
	onPlayerPickedBonusFunc = scriptTable["onPlayerPickedBonus"];
	onMonsterDamagedFunc = scriptTable["onMonsterDamaged"];

	reset();
}

void Perk::takeLevel()
{
	if (level < maxLevel || maxLevel == -1)
	{
		level++;
		if (maxLevel == 1)
		{
			scriptTable["init"]();
		}
		else
		{
			scriptTable["init"](level);
		}
	}
}

void Perk::reset()
{
	scriptTable["data"] = lua.create_table();
	if (scriptTable["reset"])
	{
		scriptTable["reset"]();
	}
	level = 0;
}

int Perk::onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args)
{
	if (onMonsterDamagedFunc)
	{
		return onMonsterDamagedFunc(monster, damage, dir, args);
	}
	return damage;
}

void Perk::onTick()
{
	if (onTickFunc)
	{
		onTickFunc();
	}
}

void Perk::onAddGunBullet(Gun *gun, Bullet *bullet)
{
	if (onAddGunBulletFunc)
	{
		onAddGunBulletFunc(gun, bullet);
	}
}

void Perk::onReload(Gun *gun)
{
	if (onReloadFunc)
	{
		onReloadFunc(gun);
	}
}

int Perk::onPlayerDamaged(int damage, float dir, sol::table& params)
{
	if (onPlayerDamagedFunc)
	{
		return onPlayerDamagedFunc(damage, dir, params);
	}
	return damage;
}

void Perk::onPlayerDied()
{
	if (onPlayerDiedFunc)
	{
		onPlayerDiedFunc();
	}
}

void Perk::onMonsterDied(Monster* monster)
{
	if (onMonsterDiedFunc)
	{
		onMonsterDiedFunc(monster);
	}
}

void Perk::onPlayerPickedGun(Gun *gun)
{
	if (onPlayerPickedGunFunc)
	{
		onPlayerPickedGunFunc(gun);
	}
}

void Perk::onPlayerPickedBonus(Bonus *bonus, const Vec2& pos)
{
	if (onPlayerPickedBonusFunc)
	{
		onPlayerPickedBonusFunc(bonus, pos);
	}
}
