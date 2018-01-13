#pragma once

#include "cGlobals.h"
class cParticleTemplate;
class cTexturedQuadRenderable;
class cParticle;
class Bloodworks;

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

class ExplosionController
{
	Bloodworks *bloodworks;
	cTextureShr ring;
	cParticleTemplate *fireParticle;
	cVector<ExplosionData> explosions;
	cParticle *explosionParticles;
	bool noParticles;

public:
	ExplosionController(Bloodworks *bloodworks);
	~ExplosionController();
	void tick();
	ExplosionData& addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage, float startTime = 0.0f, bool damagePlayer = false, sol::function onHit = sol::function(), bool noParticle = false );
	void reset();
};