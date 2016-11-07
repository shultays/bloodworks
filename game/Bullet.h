#pragma once

#include "cMat.h"
#include "cVec.h"

class cRenderableGroup;
class Bloodworks;
class cRenderable;

class Bullet
{
	friend class BulletController;
	friend class Gun;

	bool isDead;
	Bloodworks *bloodworks;
	Vec2 pos;
	float rotation;
	Vec2 speed;
	float radius;
	cRenderableGroup *renderable;
public:
	Bullet(Bloodworks *bloodworks);
	void init();
	~Bullet();
	void tick(float dt);
	void addRenderable(cRenderable *renderable);

};