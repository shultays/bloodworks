#pragma once

class Bloodworks;

#include "cMat.h"
#include "cVec.h"
#include "cLuaWorld.h"
#include "BuffFloat.h"

class Gun;
class Bullet;
class cParticle;
class MonsterTemplate;
class cAnimatedTexturedQuadRenderable;

class Monster
{
	friend class MonsterController;
	friend class BloodworksLuaWorld;
	friend class Bloodworks;

	cAnimatedTexturedQuadRenderable *renderable;
	cTextRenderable *healthRenderable;
	Bloodworks *bloodworks;

	std::string name;
	int id;

	Vec2 position;
	Vec2 prevPosition;
	float moveSpeed;
	float moveAngle;
	float spriteAngle;
	Vec2 moveDir;
	Vec2 moveVelocity;

	Vec2 partScale;
	Vec2 textureSize;
	Vec2 textureShift;
	int hitPoint;
	float collisionRadius;
	float bulletRadius;
	float lastBitTime;

	bool isDead;
	bool removeOnDead;
	bool hasBlood;
	bool hasGibs;
	bool firstTick;
	float animationSpeed;

	const MonsterTemplate* monsterTemplate;

	//lua
	sol::table data;
	sol::table scriptTable;
	struct Timer
	{
		bool looped;
		float time;
		std::string func;
		sol::table args;
	};
	cVector<Timer> timers;

	sol::function luaTick;
	sol::function luaHit;
	sol::function luaKill;
	sol::function luaShouldHit;

	float scale;
	//~lua

	cVector<int> ignoreIds;

	float experienceMultiplier;
	float scoreMultiplier;

	float lastHitSoundPlayTime;

	void killSelf(const Vec2& blowDir);

	int lastRunCheck;
	float lastSoundTime;

	BuffFloat knockbackResistance;
	BuffFloat moveSpeedMultiplier;
	BuffVec4 colorMultiplier;
	float dropChance;

	cVector<cParticle*> particles;

	struct Knockback 
	{
		Vec2 speed;
		float duration;
	};
	cVector<Knockback> knockbacks;

	void clampPos();
	int debugVal;

	bool hasCollision;
	bool canGetOneShooted;
public:
	Monster(Bloodworks *bloodworks);
	~Monster();
	void init(const MonsterTemplate* monsterTemplate);
	int getId();
	void setScale(float scale);
	void tick();
	void addTimer(float timeToTrigger, const std::string& func, sol::table args, bool looped = false);
	void playAnimation(std::string anim, float startPercentage);
	void doDamage(int damage, const Vec2& dir);
	void doDamageWithArgs(int damage, const Vec2& dir, sol::table& args);
	Vec2 getPathPos(const Vec2& pos);
	bool isVisible() const;
	void setVisible(bool visible);
	bool isRemoved() const
	{
		return isDead;
	}

	const Vec2& getPosition() const
	{
		return position;
	}

	const Vec2& getPreviousPosition() const
	{
		return prevPosition;
	}

	float getRadius() const
	{
		return bulletRadius;
	}

	IntVec2 gridStart;
	IntVec2 gridEnd;

	void addIgnoreId(int id);
	bool hasIgnoreId(int id) const;

	void copyIgnoreId(Monster *other);
	void setPosition(const Vec2& pos);
	bool shouldHit(Bullet *bullet);
	bool shouldHit(Gun *gun);
	cParticle* addParticleSpawner(const std::string& name, sol::table& args);


	void spawnParticle(cParticle *particle, sol::table& params);
	void spawnParticleShifted(const Vec2& shift, cParticle *particle, sol::table& params);

	void spawnParticleInternal(cParticle *particle, sol::table* params);
	void spawnParticleShiftedInternal(const Vec2& shift, cParticle *particle, sol::table* params);

	void spawnParticleWithoutArgs(cParticle *particle);
	void spawnParticleShiftedWithoutArgs(const Vec2& shift, cParticle *particle);

	void addKnockback(const Vec2& speed, float duration);
	void setDrawLevel(int level);
	void modifyDrawLevel(int level);
	void spawnBodyParts(const Vec2& blowDir);
	void spawnBits(const Vec2& blowDir, int extraBits = 0);
	void setDebug(int debug)
	{
		this->debugVal = debug;
	}
	int getDebug() const
	{
		return debugVal;
	}
	bool isFirstTick() const
	{
		return firstTick;
	}
};