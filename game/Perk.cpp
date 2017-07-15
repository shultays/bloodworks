#include "Perk.h"
#include "Gun.h"
#include "Monster.h"
#include "Bullet.h"
#include "cGlobals.h"

Perk::Perk(nlohmann::json& j)
{
	name = j["name"].get<std::string>();
	description = j["description"].get<std::string>();
	iconPath = j["iconFile"].get<std::string>();
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

	std::string scriptPath = j["scriptFile"].get<std::string>();
	lua.script_file(scriptPath);

	onAddGunBulletFunc = scriptTable["onAddGunBullet"];
	onPlayerDamagedFunc = scriptTable["onPlayerDamaged"];
	onTickFunc = scriptTable["onTick"];
	onReloadFunc = scriptTable["onReload"];

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

void Perk::reset()
{
	if (scriptTable["reset"])
	{
		scriptTable["reset"]();
	}
	level = 0;
}

void Perk::onPlayerDied()
{
	if (scriptTable["onPlayerDied"])
	{
		scriptTable["onPlayerDied"]();
	}
}

void Perk::onMonsterDied(Monster* monster)
{
	if (scriptTable["onMonsterDied"])
	{
		scriptTable["onMonsterDied"](monster);
	}
}
