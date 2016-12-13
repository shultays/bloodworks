#include "Bullet.h"
#include "cRenderable.h"
#include "Monster.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "Gun.h"
#include "cParticle.h"
#include "MonsterController.h"
#include "Player.h"

Bullet::Bullet(Bloodworks *bloodworks, Gun *gun)
{
	this->bloodworks = bloodworks;
	this->gun = gun;
	renderable = new cRenderableGroup(bloodworks);
	bloodworks->addRenderable(renderable, BULLETS);
	isDead = false;
	id = bloodworks->getUniqueId();
	damage = 10;
	diesOnHit = true;
	meshScale = Vec2(1.0f);
	data = lua.create_table();
	meshRotation = -1500;
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

void Bullet::tick()
{
	float dt = timer.getDt();
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
	moveDir = Vec2::fromAngle(moveAngle);
	moveSpeedDir = moveDir * moveSpeed * bloodworks->getPlayer()->getBulletSpeedMultiplier();

	pos += moveSpeedDir * dt;

	updateDrawable();

	if (bloodworks->isCoorOutside(pos, radius + 40.0f))
	{
		removeSelf();
	}

	if (isDead)
	{
		return;
	}

	auto table = lua.create_table();
	table["bullet"] = this;
	for (auto& particleData : particles)
	{
		particleData.moveDistanceSinceSpawn += moveSpeed * dt;
		if (particleData.moveDistanceSinceSpawn > particleData.spawnDistance)
		{
			particleData.moveDistanceSinceSpawn -= particleData.spawnDistance;
			particleData.particle->addParticle(pos + (particleData.spawnShift * Mat2::rotation(-getMeshRotation() + pi_d2)), table);
		}
	}

	const auto& monsters = bloodworks->getMonsterController()->getMonsterAt(pos);
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
	mat.scaleBy(meshScale);
	mat.rotateBy(-getMeshRotation());
	mat.translateBy(pos);
	renderable->setWorldMatrix(mat);
}


void Bullet::removeSelf()
{
	isDead = true;
	renderable->setVisible(false);
}

float Bullet::getMeshRotation()
{
	return (meshRotation > -1000 ? meshRotation : moveAngle);
}

void Bullet::addRenderableTextureWithPosAndSize(const std::string& texture, const Vec2& pos, const Vec2& dimensions)
{
	cTexturedQuadRenderable* quad = new cTexturedQuadRenderable(bloodworks, texture, "resources/default");
	quad->setWorldMatrix(Mat3::scaleMatrix(dimensions.isNonZero() ? dimensions : quad->getTexture()->getDimensions().toVec()).translateBy(pos));
	addRenderable(quad);
	updateDrawable();
}


cParticle* Bullet::addTrailParticle(const std::string& name, const Vec2& shift, float spawnDistance, const sol::table& args)
{
	Particledata particleData;
	particleData.particle = new cParticle(bloodworks, bloodworks->getParticleTemplate(name), args);
	particleData.spawnShift = shift;
	particleData.moveDistanceSinceSpawn = spawnDistance;
	particleData.spawnDistance = spawnDistance;
	bloodworks->addRenderable(particleData.particle, BULLETS - 1);
	particles.push_back(particleData);
	return particleData.particle;
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
