#include "Bonus.h"

#include "cTools.h"
#include "cGlobals.h"
#include "json.h"
#include "Gun.h"
#include "Monster.h"
#include "Bullet.h"
#include "Bloodworks.h"

Bonus::Bonus(Bloodworks *bloodworks, nlohmann::json& j, const DirentHelper::File& file)
{
	this->bloodworks = bloodworks;

	name = j["name"].get<std::string>();
	iconPath = file.folder + j["iconFile"].get<std::string>();
	scriptName = j["scriptName"].get<std::string>();
	std::string scriptFile = file.folder + j["scriptFile"].get<std::string>();

	spawnChance = 1.0f;
	if (j.count("spawnChance"))
	{
		spawnChance = j["spawnChance"].get<float>();
	}
	lua[scriptName] = lua.create_table();
	sol::table scriptTable = lua[scriptName];
	std::string folder = file.folder;
	fixFolderPath(folder);
	scriptTable["basePath"] = folder;
	lua.script_file(scriptFile);

	spawnFunc = scriptTable["spawn"];

	dynamicSpawnChance = scriptTable["variableBonusChance"];
	onAddGunBulletFunc = scriptTable["onAddGunBullet"];
	onPlayerDamagedFunc = scriptTable["onPlayerDamaged"];
	onTickFunc = scriptTable["onTick"];
	onReloadFunc = scriptTable["onReload"];
	onMonsterDiedFunc = scriptTable["onMonsterDied"];
	onPlayerDiedFunc = scriptTable["onPlayerDied"];
	onPlayerPickedGunFunc = scriptTable["onPlayerPickedGun"];
	onPlayerPickedBonusFunc = scriptTable["onPlayerPickedBonus"];
	onMonsterDamagedFunc = scriptTable["onMonsterDamaged"];
	
	active = false;
}

void Bonus::setActive(bool active)
{
	if (this->active != active)
	{
		this->active = active;
		if (active)
		{
			bloodworks->addToActiveBonuses(this);
		}
		else
		{
			bloodworks->removeFromActiveBonuses(this);
		}
	}
}

void Bonus::spawnAt(const Vec2& pos)
{
	spawnFunc(this, pos);
}

void Bonus::reset()
{
	active = false;
	if (lua[scriptName]["clear"])
	{
		lua[scriptName]["clear"]();
	}
	data = lua.create_table();
	lua[scriptName]["data"] = data;
}

float Bonus::getSpawnChance()
{
	float chance = spawnChance;
	if (dynamicSpawnChance)
	{
		chance = dynamicSpawnChance(this);
	}
	return chance * 2.0f;
}

void Bonus::onTick()
{
	if (onTickFunc)
	{
		onTickFunc(this);
	}
}

void Bonus::onAddGunBullet(Gun *gun, Bullet *bullet)
{
	if (onAddGunBulletFunc)
	{
		onAddGunBulletFunc(this, gun, bullet);
	}
}

void Bonus::onReload(Gun *gun)
{
	if (onReloadFunc)
	{
		onReloadFunc(this, gun);
	}
}

int Bonus::onPlayerDamaged(int damage, float dir, sol::table& params)
{
	if (onPlayerDamagedFunc)
	{
		return onPlayerDamagedFunc(this, damage, dir, params);
	}
	return damage;
}

void Bonus::onPlayerDied()
{
	if (onPlayerDiedFunc)
	{
		onPlayerDiedFunc(this);
	}
}

void Bonus::onMonsterDied(Monster* monster)
{
	if (onMonsterDiedFunc)
	{
		onMonsterDiedFunc(this, monster);
	}
}

void Bonus::onPlayerPickedGun(Gun *gun)
{
	if (onPlayerPickedGunFunc)
	{
		onPlayerPickedGunFunc(this, gun);
	}
}

void Bonus::onPlayerPickedBonus(Bonus *bonus, const Vec2& pos)
{
	if (onPlayerPickedBonusFunc)
	{
		onPlayerPickedBonusFunc(this, bonus, pos);
	}
}

int Bonus::onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args)
{
	if (onMonsterDamagedFunc)
	{
		return onMonsterDamagedFunc(this, monster, damage, dir, args);
	}
	return damage;
}
