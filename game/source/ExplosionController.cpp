#include "ExplosionController.h"
#include "Bloodworks.h"
#include "cParticle.h"
#include "cTexturedQuadRenderable.h"
#include "MonsterController.h"
#include "BloodworksConfig.h"
#include "cTools.h"

ExplosionController::ExplosionController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	ring = resources.getTexture("resources/particles/explosionFire/ring.png");
	fireParticle = bloodworks->getParticleTemplate("ExplosionFireParticle");
	explosionParticles = new cParticle(bloodworks, fireParticle, lua.create_table());
	explosionParticles->setWorldMatrix(Mat3::identity());
	bloodworks->addRenderable(explosionParticles, MONSTERS + 1);

	noParticles = bloodworks->getConfig()->getBool("no_explosion_particle", false, "Disables explosion particles (there is a problem with explosion particles on some machines)");
}

ExplosionController::~ExplosionController()
{
	reset();
	ring = nullptr;
	SAFE_DELETE(explosionParticles);
}

void ExplosionController::tick()
{
	for (int i = 0; i < explosions.size(); i++)
	{
		auto& explosionData = explosions[i];
		float dt = timer.getTime() - explosionData.startTime;

		float newScale = dt * explosionData.scaleSpeed;

		if (newScale > explosionData.lastDamageScale || newScale > explosionData.maxScale)
		{
			explosionData.lastDamageScale = newScale + 15.0f;
			float damageScale = min(newScale + 15.0f, explosionData.maxScale);
			bloodworks->getMonsterController()->damageMonstersInRangeWithIgnoreId(explosionData.pos, damageScale, explosionData.minDamage, explosionData.maxDamage, true, explosionData.id);
		}

		if (newScale > explosionData.maxScale)
		{
			SAFE_DELETE(explosions[i].ringRenderable);
			explosions.swapToTailRemove(i);
			i--;
			continue;
		}

		float alpha = 0.0f;
		float a0 = min(20.0f, explosionData.maxScale * 0.2f);
		float a1 = min(50.0f, explosionData.maxScale * 0.5f);
		float a2 = max(explosionData.maxScale - 50.0f, explosionData.maxScale * 0.8f);
		a2 = max(a2, a1);
		if (newScale < a0)
		{
			alpha = 0.0f;
		}
		else if (newScale < a1)
		{
			alpha = (newScale - a0) / (a1 - a0);
		}
		else if (newScale < a2)
		{
			alpha = 1.0f;
		}
		else
		{
			alpha = (explosionData.maxScale - newScale) / (explosionData.maxScale - a2);
		}
		explosionData.ringRenderable->setWorldMatrix(Mat3::scaleMatrix(newScale + 10.0f).translateBy(explosionData.pos));
		explosionData.ringRenderable->setColor(Vec4(1.0f, 1.0f, 1.0f, alpha * 0.4f));
	}
}

void ExplosionController::addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage, float startTime)
{
	float particleScale = maxScale * 0.67f;
	float duration = maxScale / scaleSpeed;

	static int fadeoutTimeIndex = explosionParticles->getParticleTemplate()->getAttributeIndex("fadeoutTime");
	static int scaleSpeedIndex = explosionParticles->getParticleTemplate()->getAttributeIndex("scaleSpeed");
	cParticleRandomizer r;
	r.addLinear(fadeoutTimeIndex, duration - 0.2f, duration + 0.2f);
	r.addLinear(scaleSpeedIndex, particleScale / duration, particleScale / duration + 6.0f);

	if (noParticles == false)
	{
		for (int i = 0; i < 15; i++)
		{
			explosionParticles->addParticleInternal(pos, nullptr, &r);
		}
	}

	ExplosionData explosionData;
	explosionData.ringRenderable = new cTexturedQuadRenderable(bloodworks, "resources/particles/explosionFire/ring.png", "resources/default");
	explosionData.ringRenderable->setWorldMatrix(Mat3::scaleMatrix(0.0f).translateBy(pos));
	explosionData.ringRenderable->setColor(Vec4(0.0f));

	explosionData.lastDamageScale = -50.0f;
	explosionData.maxScale = maxScale;
	explosionData.scaleSpeed = scaleSpeed;
	explosionData.startTime = timer.getTime() - startTime;
	explosionData.minDamage = minDamage;
	explosionData.maxDamage = maxDamage;

	explosionData.pos = pos;
	explosionData.id = bloodworks->getUniqueId();
	bloodworks->addRenderable(explosionData.ringRenderable, MONSTERS + 1);

	explosions.push_back(explosionData);
}

void ExplosionController::reset()
{
	for (auto& explosionData : explosions)
	{
		SAFE_DELETE(explosionData.ringRenderable);
	}
	explosions.clear();
	explosionParticles->clear();
}
