#include "Bullet.h"
#include "cRenderable.h"
#include "Monster.h"
#include "Bloodworks.h"
#include "cTexture.h"
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
	diesOnHit = true;

	data = lua.create_table();
}

void Bullet::init()
{
}

Bullet::~Bullet()
{
	SAFE_DELETE(renderable);
}

void Bullet::tick(float dt)
{
	Vec2 oldPos = pos;

	if (script && onTickCallback.size())
	{
		script[onTickCallback](this);
	}
	Vec2 dir = Vec2::fromAngle(rotation);
	pos += dir * speed * dt;

	updateDrawable();

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
			if (script && shouldHitMonsterTest.size())
			{
				if (((bool)script[shouldHitMonsterTest](this, monster)) == false)
				{
					continue;
				}
			}

			if (gun != nullptr)
			{
				if (gun->getScriptTable()["onBulletHit"])
				{
					gun->getScriptTable()["onBulletHit"](this, monster);
				}
			}

			if (script && onHitCallback.length())
			{
				script[onHitCallback](this, monster);
			}

			monster->doDamage(damage, dir);
			isDead = diesOnHit;
			break;
		}
	}
}

void Bullet::addRenderable(cRenderable *renderable)
{
	this->renderable->addRenderable(renderable);
}

void Bullet::updateDrawable()
{
	Mat3 mat = Mat3::identity();
	mat.rotateBy(-rotation);
	mat.translateBy(pos);
	renderable->setWorldMatrix(mat);
}


void Bullet::addRenderableTextureWithPosAndSize(const std::string& texture, const Vec2& pos, const Vec2& dimensions)
{
	cTexturedQuadRenderable* quad = new cTexturedQuadRenderable(bloodworks, texture.c_str(), "resources/default");
	quad->setWorldMatrix(Mat3::scaleMatrix(dimensions.isNonZero() ? dimensions : quad->getTexture()->getDimensions().toVec()).translateBy(pos));
	addRenderable(quad);
	updateDrawable();
}


void Bullet::addRenderableTextureWithSize(const std::string& texture, const Vec2& dimensions)
{
	addRenderableTextureWithPosAndSize(texture, Vec2::zero(), dimensions);
}

void Bullet::addRenderableTexture(const std::string& texture)
{
	addRenderableTextureWithPosAndSize(texture, Vec2::zero(), Vec2::zero());
}
