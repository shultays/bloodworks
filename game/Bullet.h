#pragma once

#include "cMat.h"
#include "cVec.h"

class cRenderableGroup;
class Bloodworks;
class cRenderable;

class Bullet
{
	friend class BulletController;
	friend class MissionController;
	friend class Gun;

	Bloodworks *bloodworks;
	Gun *gun;

	bool isDead;
	Vec2 pos;
	float rotation;
	float speed;

	float radius;
	int damage;
	cRenderableGroup *renderable;
	int id;
	static int nextId;

	std::string onHitCallback;
	std::string onTickCallback;
	std::string ignoreMonsterCallback;
public:
	Bullet(Bloodworks *bloodworks, Gun *gun);
	~Bullet();
	void init();
	void tick(float dt);
	void addRenderable(cRenderable *renderable);
	int getId()
	{
		return id;
	}

	const Vec2& getPosition()
	{
		return pos;
	}

	float getRadius()
	{
		return radius;
	}

	IntVec2 gridStart;
	IntVec2 gridEnd;

	void addRenderableTexture(const std::string& texture);

};