#include "BulletController.h"
#include "Bullet.h"

#include "cGlobals.h"
#include "lua.h"
#include "Gun.h"

void BulletController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	lua.new_usertype<Bullet>("Bullet",
		"index", sol::readonly(&Bullet::id),

		"position", &Bullet::pos,
		"moveSpeed", &Bullet::moveSpeed,
		"moveAngle", &Bullet::moveAngle,
		"meshRotation", &Bullet::meshRotation,

		"radius", &Bullet::radius,
		"damage", &Bullet::damage,

		"diesOnHit", &Bullet::diesOnHit,
		"updateDrawable", &Bullet::updateDrawable,

		"script", &Bullet::script,

		"onHitCallback", &Bullet::onHitCallback,
		"onTickCallback", &Bullet::onTickCallback,
		"shouldHitMonsterTest", &Bullet::shouldHitMonsterTest,

		"isDead", sol::readonly(&Bullet::isDead),

		"data", &Bullet::data,

		"addRenderableTexture", &Bullet::addRenderableTexture,
		"addRenderableTextureWithSize", &Bullet::addRenderableTextureWithSize,
		"addRenderableTextureWithPosAndSize", &Bullet::addRenderableTextureWithPosAndSize,
		"addTrailParticle", &Bullet::addTrailParticle,

		"removeSelf", &Bullet::removeSelf
		);


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
		if (bullets[i]->isDead == false)
		{
			bullets[i]->tick(dt);
		}

		if (bullets[i]->isDead)
		{
			if (bullets[i]->hasParticles() == false)
			{
				grid.removeFromGrid(bullets[i]);
				int id = bullets[i]->getId();
				SAFE_DELETE(bullets[i]);
				bulletMap.erase(id);
				bullets[i] = bullets[bullets.size() - 1];
				bullets.resize(bullets.size() - 1);
				i--;
			}
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

