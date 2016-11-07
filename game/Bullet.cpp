#include "Bullet.h"
#include "cRenderable.h"
#include "Monster.h"
#include "Bloodworks.h"

Bullet::Bullet(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	renderable = new cRenderableGroup(bloodworks);
	bloodworks->addRenderable(renderable, 501);
	isDead = false;
}

void Bullet::init()
{
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
	

	const auto& monsters = bloodworks->getMonsterController()->getMonsterAt(pos);

	for (auto& monster : monsters)
	{
		Vec2 monsterPos = monster->getPosition();
		float radiusToCheck = monster->getRadius() + radius;
		if (pos.distanceSquared(monsterPos) < radiusToCheck * radiusToCheck)
		{
			monster->doDamage(randInt(10, 30));
			isDead = true;
		}
	}

	if (pos.x > 400 || pos.x < -400 || pos.y > 300 || pos.y < -300)
	{
		isDead = true;
	}
}

void Bullet::addRenderable(cRenderable *renderable)
{
	this->renderable->addRenderable(renderable, Mat3::identity());
}
