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

	Bloodworks *bloodworks;
	Gun *gun;

	bool isDead;
	Vec2 pos;
	float rotation;
	Vec2 speed;
	float radius;
	int damage;
	cRenderableGroup *renderable;
	int id;
	static int nextId;

	std::string onHitCallback;
public:
	Bullet(Bloodworks *bloodworks, Gun *gun);
	void init();
	~Bullet();
	void tick(float dt);
	void addRenderable(cRenderable *renderable);
	void addBulletOnHitCallback(const std::string& onHitCallback);
	int getId()
	{
		return id;
	}
};