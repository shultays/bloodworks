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

	grid.init(bloodworks->getMapMin() - 550.0f, bloodworks->getMapSize() + 1100.0f, Vec2(50.0f));
}

BulletController::~BulletController()
{
	reset();
}

void BulletController::addBullet(Bullet* bullet)
{
	bullets.push_back(bullet);
	bulletMap[bullet->getId()] = bullet;
	grid.insertToGrid(bullet);
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
			grid.removeFromGrid(bullets[i]);
			int id = bullets[i]->getId();
			SAFE_DELETE(bullets[i]);
			bulletMap.erase(id);
			bullets[i] = bullets[bullets.size() - 1];
			bullets.resize(bullets.size() - 1);
			i--;
		}
		else
		{
			grid.relocate(bullets[i]);
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
	bloodworks->getBulletController()->addBullet(bullet);

	return bullet;
}

void BulletController::reset()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		grid.removeFromGrid(bullets[i]);
		SAFE_DELETE(bullets[i]);
	}
	bullets.clear();
	bulletMap.clear();
}

void BulletController::relocateBullet(Bullet* bullet)
{
	grid.relocate(bullet);
}

void BulletController::drawDebug()
{
	grid.drawDebug();
}
