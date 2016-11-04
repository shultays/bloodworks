#pragma once

#include "cMat.h"
#include "cVec.h"
#include "cRenderable.h"

class Bloodworks;

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
public:
	Player(Bloodworks *bloodworks);
	~Player();
	void tick(float dt);

	const Vec2& getPos() const
	{
		return pos;
	}


};