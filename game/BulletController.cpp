#include "BulletController.h"
#include "Bullet.h"

void BulletController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
}

BulletController::~BulletController()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		SAFE_DELETE(bullets[i]);
	}
}

void BulletController::addBullet(Bullet* bullet)
{
	bullets.push_back(bullet);
}

void BulletController::tick(float dt)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i]->tick(dt);
		if (bullets[i]->isDead)
		{
			SAFE_DELETE(bullets[i]);
			bullets[i] = bullets[bullets.size() - 1];
			bullets.resize(bullets.size() - 1);
			i--;
		}
	}
}

