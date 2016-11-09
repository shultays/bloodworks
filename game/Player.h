#pragma once

#include "cMat.h"
#include "cVec.h"
#include "cRenderable.h"

class Bloodworks;
class Gun;

class Player
{
	Vec2 pos;
	float angle;

	float moveAngle;
	float moveSpeed;

	Bloodworks *bloodworks;
	cRenderableGroup *renderable;
	cRenderable *crosshair;
	Vec2 crosshairPos;
	Gun *gun;
	Vec2 aimDir;
	float oldSpreadAngle;

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
};