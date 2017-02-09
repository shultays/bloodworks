#pragma once

class Bloodworks;

#include "cMat.h"
#include "cVec.h"
#include "cLuaWorld.h"
#include "cAnimatedRenderable.h"
#include "MonsterTemplate.h"
#include "BuffFloat.h"

class Bullet;

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

	Vec2 textureSize;
	Vec2 textureShift;
	int hitPoint;
	float collisionRadius;
	float bulletRadius;
	float lastBitTime;

	bool isDead;
	bool hasBlood;

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

	float lastHitSoundPlayTime;

	void killSelf(const Vec2& blowDir);
	void spawnBits(const Vec2& position, const Vec2& blowDir, int extraBits = 0);

	int lastRunCheck;

	BuffFloat moveSpeedMultiplier;
	BuffVec4 colorMultiplier;
	float dropChance;
public:
	Monster(Bloodworks *bloodworks);
	~Monster();
	void init(const MonsterTemplate* monsterTemplate);
	int getId();
	void setScale(float scale);
	void setColor(int color);
	void tick();
	void addTimer(float timeToTrigger, const std::string& func, sol::table args, bool looped = false);
	void playAnimation(std::string anim, float startPercentage);
	void doDamage(int damage, const Vec2& dir);
	void doDamageWithArgs(int damage, const Vec2& dir, sol::table& args);
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

	void addIgnoreId(int id);
	bool hasIgnoreId(int id);

	void copyIgnoreId(Monster *other);
	void setPosition(const Vec2& pos);
	bool shouldHit(Bullet *bullet);
};