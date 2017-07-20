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

	cAnimatedTexturedQuadRenderable *renderable;
	cTextRenderable *healthRenderable;
	Bloodworks *bloodworks;

	std::string name;
	int id;

	Vec2 position;
	float moveSpeed;
	float moveAngle;
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
	bool hasBlood;
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
	std::vector<Timer> timers;

	sol::function luaTick;
	float scale;
	//~lua

	std::vector<int> ignoreIds;

	float experienceMultiplier;
	float scoreMultiplier;

	float lastHitSoundPlayTime;

	void killSelf(const Vec2& blowDir);
	void spawnBits(const Vec2& position, const Vec2& blowDir, int extraBits = 0);

	int lastRunCheck;
	float lastSoundTime;

	BuffFloat knockbackResistance;
	BuffFloat moveSpeedMultiplier;
	BuffVec4 colorMultiplier;
	float dropChance;

	std::vector<cParticle*> particles;

	struct Knockback 
	{
		Vec2 speed;
		float duration;
	};
	std::vector<Knockback> knockbacks;

	void clampPos();
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
	bool isRemoved()
	{
		return isDead;
	}

	const Vec2& getPosition()
	{
		return position;
	}

	float getRadius()
	{
		return bulletRadius;
	}

	IntVec2 gridStart;
	IntVec2 gridEnd;
	bool hasShouldHitScript;

	void addIgnoreId(int id);
	bool hasIgnoreId(int id);

	void copyIgnoreId(Monster *other);
	void setPosition(const Vec2& pos);
	bool shouldHit(Bullet *bullet);
	bool shouldHit(Gun *gun);
	cParticle* addParticleSpawner(const std::string& name, sol::table& args);
	void spawnParticle(cParticle *particle, sol::table& params);
	void addKnockback(const Vec2& speed, float duration);
	void modifyDrawLevel(int level);
};