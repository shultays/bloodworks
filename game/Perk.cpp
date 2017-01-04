#include "Perk.h"
#include "Gun.h"
#include "Bullet.h"


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

int Perk::onPlayerDamaged(int damage, sol::table& params)
{
	if (onPlayerDamagedFunc)
	{
		return onPlayerDamagedFunc(damage, params);
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
