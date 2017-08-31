#pragma once

#include "cMat.h"
#include "cVec.h"
#include "sol.h"
#include "cVector.h"

class cRenderableContainer;
class Bloodworks;
class cRenderable;
class cParticle;
class Monster;

class Bullet
{
	friend class BulletController;
	friend class Gun;
	friend class BloodworksLuaWorld;

	Bloodworks *bloodworks;
	Gun *gun;
	bool hasCollision;
	bool isDead;
	Vec2 pos;
	float moveAngle;
	float moveSpeed;
	
	float meshRotation;

	Vec2 moveDir;
	Vec2 moveVelocity;

	Vec2 meshScale;

	float scale;
	float radius;
	float startTime;
	float lifeTime;
	int damage;
	cRenderableContainer *renderable;
	int id;
	bool monsterBullet;

	sol::table script;

	sol::function onHitCallback;
	sol::function onTickCallback;
	sol::function shouldHitMonsterTest;

	sol::table data;
	sol::table particleArgs;
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

	cVector<Particledata> particles;

	void clampPos();
	void onHit(Monster* monster);
public:
	Bullet(Bloodworks *bloodworks, Gun *gun);
	~Bullet();
	void tick();
	void addRenderable(cRenderable *renderable);
	int getId() const
	{
		return id;
	}

	const Vec2& getPosition() const
	{
		return pos;
	}

	float getRadius() const
	{
		return radius * scale;
	}

	IntVec2 gridStart;
	IntVec2 gridEnd;

	void addRenderableTexture(const std::string& texture);
	void addRenderableTextureWithSize(const std::string& texture, const Vec2& dimensions);
	void addRenderableTextureWithPosAndSize(const std::string& texture, const Vec2& pos, const Vec2& dimensions);
	cParticle* addTrailParticle(const std::string& name, const Vec2& shift, float spawnDistance, const sol::table& args);
	void modifyDrawLevel(int level);
	void setPosition(const Vec2& pos);
	void setScript(sol::table& script);
	void updateDrawable();
	void removeSelf();
	float getMeshRotation();
	void setColor(const Vec4& color);
	Gun* getGun() const;
};