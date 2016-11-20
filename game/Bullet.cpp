#include "Bullet.h"
#include "cRenderable.h"
#include "Monster.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "Gun.h"
#include "cParticle.h"

Bullet::Bullet(Bloodworks *bloodworks, Gun *gun)
{
	this->bloodworks = bloodworks;
	this->gun = gun;
	renderable = new cRenderableGroup(bloodworks);
	bloodworks->addRenderable(renderable, 501);
	isDead = false;
	id = bloodworks->getUniqueId();
	damage = 10;
	diesOnHit = true;

	data = lua.create_table();
}

void Bullet::init()
{
	moveDir = Vec2::fromAngle(rotation);
	moveSpeedDir = moveDir * speed;
	meshRotation = rotation;
}

Bullet::~Bullet()
{
	SAFE_DELETE(renderable);
	for (int i = 0; i < particles.size(); i++)
	{
		SAFE_DELETE(particles[i].particle);
	}
	particles.clear();
}

void Bullet::tick(float dt)
{
	Vec2 oldPos = pos;

	if (script && onTickCallback.size())
	{
		script[onTickCallback](this);
	}

	if (gun != nullptr)
	{
		if (gun->getScriptTable()["onBulletTick"])
		{
			gun->getScriptTable()["onBulletTick"](gun, this);
		}
	}
	moveDir = Vec2::fromAngle(rotation);
	moveSpeedDir = moveDir * speed;;

	pos += moveSpeedDir * dt;

	updateDrawable();

	const auto& monsters = bloodworks->getMonsterController()->getMonsterAt(pos);

	if (pos.x > 500 || pos.x < -500 || pos.y > 400 || pos.y < -400)
	{
		removeSelf();
	}

	if (isDead)
	{
		return;
	}
	
	for (auto& particleData : particles)
	{
		particleData.particle->addParticle(pos + (particleData.spawnShift * Mat2::rotation(-meshRotation + pi_d2)), lua.create_table());
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
					gun->getScriptTable()["onBulletHit"](gun, this, monster);
				}
			}

			if (script && onHitCallback.length())
			{
				script[onHitCallback](this, monster);
			}

			monster->doDamage(damage, moveDir);
			if (diesOnHit)
			{
				removeSelf();
			}
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
	mat.rotateBy(-meshRotation);
	mat.translateBy(pos);
	renderable->setWorldMatrix(mat);
}


void Bullet::removeSelf()
{
	isDead = true;
	renderable->setVisible(false);
}

void Bullet::addRenderableTextureWithPosAndSize(const std::string& texture, const Vec2& pos, const Vec2& dimensions)
{
	cTexturedQuadRenderable* quad = new cTexturedQuadRenderable(bloodworks, texture.c_str(), "resources/default");
	quad->setWorldMatrix(Mat3::scaleMatrix(dimensions.isNonZero() ? dimensions : quad->getTexture()->getDimensions().toVec()).translateBy(pos));
	addRenderable(quad);
	updateDrawable();
}


void Bullet::addTrailParticle(const std::string& name, const Vec2& shift, const sol::table& args)
{
	Particledata particleData;
	particleData.particle = new cParticle(bloodworks, bloodworks->getParticleTemplate(name), args);
	particleData.spawnShift = shift;
	bloodworks->addRenderable(particleData.particle, 503);
	particles.push_back(particleData);
}

bool Bullet::hasParticles()
{
	for (auto& particleData : particles)
	{
		if (particleData.particle->hasParticle())
		{
			return true;
		}
	}
	return false;
}

void Bullet::addRenderableTextureWithSize(const std::string& texture, const Vec2& dimensions)
{
	addRenderableTextureWithPosAndSize(texture, Vec2::zero(), dimensions);
}

void Bullet::addRenderableTexture(const std::string& texture)
{
	addRenderableTextureWithPosAndSize(texture, Vec2::zero(), Vec2::zero());
}
