#include "Bullet.h"
#include "cRenderable.h"
#include "Monster.h"
#include "Bloodworks.h"
#include "Gun.h"

int Bullet::nextId = 0;

Bullet::Bullet(Bloodworks *bloodworks, Gun *gun)
{
	this->bloodworks = bloodworks;
	this->gun = gun;
	renderable = new cRenderableGroup(bloodworks);
	bloodworks->addRenderable(renderable, 501);
	isDead = false;
	id = nextId++;
	damage = 10;
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

	if (onTickCallback.size())
	{
		lua[onTickCallback](this);
	}
	Vec2 dir = Vec2::fromAngle(rotation);
	pos += dir * speed * dt;

	Mat3 mat = Mat3::identity();
	mat.scaleBy(15.0f);
	mat.rotateBy(-rotation);
	mat.translateBy(pos);

	renderable->setWorldMatrix(mat);
	
	const auto& monsters = bloodworks->getMonsterController()->getMonsterAt(pos);

	if (pos.x > 400 || pos.x < -400 || pos.y > 300 || pos.y < -300)
	{
		isDead = true;
	}

	if (isDead)
	{
		return;
	}

	for (auto& monster : monsters)
	{
		Vec2 monsterPos = monster->getPosition();
		float radiusToCheck = monster->getRadius() + radius;
		if (monster->isRemoved() == false && pos.distanceSquared(monsterPos) < radiusToCheck * radiusToCheck)
		{
			if (gun != nullptr)
			{
				if (gun->getScriptTable()["onBulletHit"])
				{
					gun->getScriptTable()["onBulletHit"](this, monster);
				}
			}

			if (onHitCallback.length())
			{
				lua[onHitCallback](this, monster);
			}

			monster->doDamage(damage);
			isDead = true;
			break;
		}
	}

}

void Bullet::addRenderable(cRenderable *renderable)
{
	this->renderable->addRenderable(renderable, Mat3::identity());
}

void Bullet::addRenderableTexture(const std::string& texture)
{
	addRenderable(new cTexturedQuadRenderable(bloodworks, texture.c_str(), "resources/default"));
}
