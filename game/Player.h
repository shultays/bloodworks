#pragma once

#include "cMat.h"
#include "cVec.h"
#include "cRenderable.h"

class Bloodworks;
class Gun;

class Player
{
	Vec2 pos;

	float moveAngle;
	float moveSpeed;

	Bloodworks *bloodworks;
	cRenderableGroup *renderable;
	cRenderable *crosshair;
	cRenderable *spread;
	Vec2 crosshairPos;
	Gun *gun;
	Vec2 aimDir;
	float aimAngle;
	float oldSpreadAngle;
	int hitPoints;
	cTextRenderable *healthRenderable;

	float slowdownAmount;
	float slowdownDuration;
public:
	Player(Bloodworks *bloodworks);
	~Player();
	void tick(float dt);

	const Vec2& getPos() const
	{
		return pos;
	}

	const Vec2& getAimDir() const
	{
		return aimDir;
	}

	void setGun(Gun* gun);
	Gun* getGun();

	void doDamage(int damage);
	void slowdown(float slowdownAmount, float slowdownDuration);
private:
	void updateHitPoints();
};