#pragma once

class Bloodworks;

#include "cMat.h"
#include "cVec.h"
#include "cLuaWorld.h"
#include "cAnimatedRenderable.h"
#include "MonsterTemplate.h"

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

	Vec2 textureSize;
	Vec2 textureShift;
	int hitPoint;
	float collisionRadius;
	float bulletRadius;

	bool isDead;
	bool hasBlood;

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
private:
	void killSelf(const Vec2& blowDir);
};