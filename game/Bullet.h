#pragma once

#include "cMat.h"
#include "cVec.h"

class cTexturedQuadRenderable;
class Bloodworks;
class Bullet
{
	friend class Bloodworks;

	bool isDead;
	Bloodworks *bloodworks;
	Vec2 pos;
	float rotation;
	Vec2 speed;
	float radius;
	cTexturedQuadRenderable *renderable;
public:
	Bullet(Bloodworks *bloodworks, const Vec2& pos, const Vec2& speed, float radius = 0.0f);
	~Bullet();
	void tick(float dt);

};