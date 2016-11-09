#include "BulletController.h"
#include "Bullet.h"

#include "cGlobals.h"
#include "lua.h"

void BulletController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	lua.set_function("addBulletOnHitCallback",
			[&](int bulletIndex, const std::string& func)
	{
		bulletMap[bulletIndex]->addBulletOnHitCallback(func);
	});

	grid.init(Vec2(-700, -600), Vec2(1400, 1200), Vec2(50, 50));
}

void BulletController::clear()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		SAFE_DELETE(bullets[i]);
	}
	bulletMap.clear();

}

BulletController::~BulletController()
{
	clear();
}

void BulletController::addBullet(Bullet* bullet)
{
	bullets.push_back(bullet);
	bulletMap[bullet->getId()] = bullet;
	grid.insertToGrid(bullet);
}

void BulletController::tick(float dt)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->tick(dt);
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

	if (input.isKeyDown(key_f2))
	{
		grid.drawDebug();
	}
}

