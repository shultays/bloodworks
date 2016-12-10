#include "Perk.h"
#include "Gun.h"
#include "Bullet.h"


void Perk::takeLevel()
{
	if (level < maxLevel)
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

int Perk::onPlayerDamaged(int damage, sol::table& params)
{
	if (onPlayerDamagedFunc)
	{
		return onPlayerDamagedFunc(damage, params);
	}
	return damage;
}
