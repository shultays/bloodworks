#include "BulletController.h"
#include "Bloodworks.h"
#include "Bullet.h"
#include "cGlobals.h"
#include "lua.h"
#include "Gun.h"
#include "Bloodworks.h"
#include "Player.h"
#include "cTimeProfiler.h"

BulletController::BulletController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	AARect rect = bloodworks->getMapLimits();
	rect.addThreshold(550.0f);

	grid.init(rect, Vec2(50.0f));
}

BulletController::~BulletController()
{
	reset();
}

void BulletController::addBullet(Bullet* bullet)
{
	bullets.push_back(bullet);
	bulletMap[bullet->getId()] = bullet;
	if (bullet->monsterBullet == false)
	{
		grid.insertToGrid(bullet);
	}
}

void BulletController::tick()
{
	ADD_SCOPED_TIME_PROFILER("BulletController::tick");
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->isDead == false)
		{
			bullets[i]->tick();
		}

		if (bullets[i]->isDead)
		{
			if (bullets[i]->monsterBullet == false)
			{
				grid.removeFromGrid(bullets[i]);
			}
			int id = bullets[i]->getId();
			SAFE_DELETE(bullets[i]);
			bulletMap.erase(id);

			bullets.swapToTailRemove(i);
			i--;
		}
		else
		{
			if (bullets[i]->monsterBullet == false)
			{
				grid.relocate(bullets[i]);
			}
		}
	}

}


Bullet* BulletController::addCustomBullet(const sol::table& params)
{
	Vec2 dir = bloodworks->getPlayer()->getAimDir();
	Bullet *bullet = new Bullet(bloodworks, nullptr);
	Player *player = bloodworks->getPlayer();

	bullet->pos = player->getPosition() + player->getAimDir() * 20.0f;
	bullet->moveSpeed = 20.0f;
	bullet->moveAngle = player->getAimDir().toAngle();
	bullet->radius = 2.0f;
	bullet->damage = 10;
	if (params)
	{
		if (params["monsterBullet"])
		{
			bullet->monsterBullet = params.get<bool>("monsterBullet");
		}
	}
	bloodworks->getBulletController()->addBullet(bullet);

	return bullet;
}

void BulletController::reset()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i]->monsterBullet == false)
		{
			grid.removeFromGrid(bullets[i]);
		}
		SAFE_DELETE(bullets[i]);
	}
	bullets.clear();
	bulletMap.clear();
}

void BulletController::relocateBullet(Bullet* bullet)
{
	if (bullet->monsterBullet == false)
	{
		grid.relocate(bullet);
	}
}

void BulletController::drawDebug()
{
	grid.drawDebug();
}
