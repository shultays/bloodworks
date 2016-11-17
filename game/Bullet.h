#pragma once

#include "cMat.h"
#include "cVec.h"
#include "sol.h"

class cRenderableGroup;
class Bloodworks;
class cRenderable;
class cParticle;

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

	sol::table script;

	std::string onHitCallback;
	std::string onTickCallback;
	std::string shouldHitMonsterTest;

	sol::table data;

	bool diesOnHit;

	struct Particledata
	{
		cParticle* particle;
		Vec2 spawnShift;
	};

	std::vector<Particledata> particles;
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
	void addRenderableTextureWithSize(const std::string& texture, const Vec2& dimensions);
	void addRenderableTextureWithPosAndSize(const std::string& texture, const Vec2& pos, const Vec2& dimensions);
	void addTrailParticle(const std::string& name, const Vec2& shift, const sol::table& args);

	bool hasParticles();
private:
	void updateDrawable();
	void removeSelf();
};