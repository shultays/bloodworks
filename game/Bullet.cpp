#include "Bullet.h"
#include "cRenderable.h"
#include "Monster.h"
#include "Bloodworks.h"

Bullet::Bullet(Bloodworks *bloodworks, const Vec2& pos, const Vec2& speed, float radius /*= 0.0f*/)
{
	this->bloodworks = bloodworks;
	this->pos = pos;
	this->speed = speed;
	this->radius = radius;

	renderable = new cTexturedQuadRenderable(bloodworks, "resources/bullet.png", "resources/default");
	bloodworks->addRenderable(renderable);

	rotation = speed.toAngle();
	isDead = false;
	tick(0.0f);

}

Bullet::~Bullet()
{
	SAFE_DELETE(renderable);
}

void Bullet::tick(float dt)
{
	Vec2 oldPos = pos;

	pos += speed * dt;

	Mat3 mat = Mat3::identity();
	mat.scaleBy(15.0f);
	mat.rotateBy(-rotation);
	mat.translateBy(pos);

	renderable->setWorldMatrix(mat);
	

	const auto& monsters = bloodworks->monsterController.getMonsterAt(pos);

	for (auto& monster : monsters)
	{
		Vec2 monsterPos = monster->getPosition();
		float radiusToCheck = monster->getRadius() + radius;
		if (pos.distanceSquared(monsterPos) < radiusToCheck * radiusToCheck)
		{
			monster->doDamage(5);
			isDead = true;
		}
	}

	if (pos.x > 400 || pos.x < -400 || pos.y > 300 || pos.y < -300)
	{
		isDead = true;
	}
}
