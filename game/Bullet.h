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
	friend class Gun;
	friend class BloodworksLuaWorld;

	Bloodworks *bloodworks;
	Gun *gun;

	bool isDead;
	Vec2 pos;
	float moveAngle;
	float moveSpeed;
	
	float meshRotation;

	Vec2 moveDir;
	Vec2 moveSpeedDir;

	Vec2 meshScale;

	float radius;
	int damage;
	cRenderableGroup *renderable;
	int id;
	bool monsterBullet;

	sol::table script;

	std::string onHitCallback;
	std::string onTickCallback;
	std::string shouldHitMonsterTest;

	sol::table data;
	sol::table onDamageArgs;

	int penetrateCount;
	int penetrateUsed;

	struct Particledata
	{
		cParticle* particle;
		Vec2 spawnShift;
		float spawnDistance;
		Vec2 lastSpawnPos;
	};

	std::vector<Particledata> particles;
public:
	Bullet(Bloodworks *bloodworks, Gun *gun);
	~Bullet();
	void tick();
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
	cParticle* addTrailParticle(const std::string& name, const Vec2& shift, float spawnDistance, const sol::table& args);
	void setPosition(const Vec2& pos);
	void updateDrawable();
	void removeSelf();
	float getMeshRotation();
	Gun* getGun();
};