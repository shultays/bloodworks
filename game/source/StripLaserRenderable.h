#pragma once

#include "Sol.h"
#include "cVec.h"

class cParticle;

class Bloodworks;

class StripLaserRenderable
{
	Bloodworks* bloodworks;

	Vec2 prevPos;
	Vec2 prevSpeed;
	float prevTime;
	bool first;
	float shootTime;
	bool shooting;
	int level;

	cParticle* particle;
	std::string particleTemplate;

	Vec2 shootSpeedOverride;
	Vec2 posOverride;
public:
	StripLaserRenderable(Bloodworks* bloodworks, std::string particleTemplate, int level);

	void tick(const Vec2& pos, const Vec2& speed);

	sol::table params;
	void stop();

	void setShooting(bool shooting);
	void setSpeed(const Vec2& speed);
	void setPos(const Vec2& pos);
	void clear();
};