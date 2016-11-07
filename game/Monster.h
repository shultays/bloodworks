#pragma once

class Bloodworks;

#include "cMat.h"
#include "cVec.h"
#include "cLuaWorld.h"
#include "cAnimatedRenderable.h"


class MonsterTemplate
{
	friend class Monster;
	Vec2 size;
	Vec2 textureShift;
	std::string name;
	int hitPoint;
	float collisionRadius;
	float bulletRadius;
	std::string script;
	sol::table scriptTable;

	std::string defaultAnimation;
	std::vector<cAnimatedTexturedQuadRenderable::AnimationData> animationData;
public:
	MonsterTemplate(const std::string& monsterData);
};

class Monster
{
	friend class MonsterController;

	cAnimatedTexturedQuadRenderable *renderable;
	cTextRenderable *healthRenderable;
	Bloodworks *bloodworks;

	Vec2 position;
	float moveSpeed;
	float moveAngle;

	Vec2 size;
	Vec2 textureShift;
	std::string name;
	int hitPoint;
	float collisionRadius;
	float bulletRadius;
	int index;
	static int nextId;


	//lua
	void setMonsterData();
	void getMonsterData();

	sol::table luaMonster;
	sol::table scriptTable;
	struct Timer
	{
		bool looped;
		float time;
		std::string func;
		sol::table args;
	};
	std::vector<Timer> timers;
	//~lua
public:
	Monster(Bloodworks *bloodworks);
	void init(const MonsterTemplate& monsterTemplate);
	~Monster();
	void tick(float dt);
	void addTimer(float timeToTrigger, const std::string& func, sol::table args, bool looped = false);
	void playAnimation(const std::string& anim);
	void doDamage(int damage);

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
};