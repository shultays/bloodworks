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
	};
	std::vector<ExplosionData> explosions;
	cParticle *explosionParticles;

public:
	ExplosionController(Bloodworks *bloodworks);
	~ExplosionController();
	void tick();
	void addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage);
};