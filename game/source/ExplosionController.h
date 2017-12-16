#pragma once

#include "cGlobals.h"
class cParticleTemplate;
class cTexturedQuadRenderable;
class cParticle;
class Bloodworks;

class ExplosionController
{
	Bloodworks *bloodworks;
	cTextureShr ring;
	cParticleTemplate *fireParticle;
	struct ExplosionData
	{
		cTexturedQuadRenderable *ringRenderable;
		int id;
		float startTime;
		float maxScale;
		float lastDamageScale;
		int minDamage;
		int maxDamage;
		float scaleSpeed;
		Vec2 pos;
		bool damagePlayer;
		bool damagedPlayer;
		sol::function onHit;
	};
	cVector<ExplosionData> explosions;
	cParticle *explosionParticles;
	bool noParticles;
public:
	ExplosionController(Bloodworks *bloodworks);
	~ExplosionController();
	void tick();
	void addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage, float startTime = 0.0f, bool damagePlayer = false, sol::function onHit = nullptr );
	void reset();
};