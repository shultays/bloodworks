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
	onDamageArgs = lua.create_table();
	onDamageArgs["bullet"] = this;
	onDamageArgs["gun"] = gun;
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
		Vec2 finalPos = pos + (particleData.spawnShift * Mat2::rotation(-getMeshRotation() + pi_d2));
		Vec2 toFinalPos = (finalPos - particleData.lastSpawnPos).normalized();
		while (particleData.lastSpawnPos.distanceSquared(finalPos) > particleData.spawnDistance * particleData.spawnDistance)
		{
			
			particleData.lastSpawnPos += toFinalPos * particleData.spawnDistance;
			if (Vec2::dot(toFinalPos, finalPos - particleData.lastSpawnPos) < 0.0f)
			{
				particleData.lastSpawnPos = finalPos;
			}

			particleData.particle->addParticle(particleData.lastSpawnPos, table);
		}
	}
	
	std::function<bool(Monster *monster)> func = [&](Monster *monster) -> bool
	{
		Vec2 monsterPos = monster->getPosition();
		float radiusToCheck = monster->getRadius() + radius;
		if (monster->isRemoved() == false && pos.distanceSquared(monsterPos) < radiusToCheck * radiusToCheck && (diesOnHit || monster->hasIgnoreId(id) == false))
		{
			if (script && shouldHitMonsterTest.size())
			{
				if (((bool)script[shouldHitMonsterTest](this, monster)) == false)
				{
					return true;
				}
			}

			if (gun)
			{
				gun->onBulletHit(this, monster);
			}

			if (script && onHitCallback.length())
			{
				script[onHitCallback](this, monster);
			}

			monster->doDamageWithArgs(damage, moveDir, onDamageArgs);
			if (diesOnHit)
			{
				removeSelf();
			}
			else
			{
				monster->addIgnoreId(id);
			}
			return false;
		}
		return true;
	};
	bloodworks->getMonsterController()->runForEachMonsterInRadius(pos, radius, func);
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
	particleData.lastSpawnPos = pos;
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
