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
	float hitPoint;

	std::string script;
	sol::table scriptTable;

	std::vector<cAnimatedTexturedQuadRenderable::AnimationData> animationData;

public:
	MonsterTemplate(const std::string& monsterData);

};

class Monster
{
	cAnimatedTexturedQuadRenderable *renderable;
	Bloodworks *bloodworks;


	Vec2 position;
	float moveSpeed;
	float moveAngle;

	Vec2 size;
	Vec2 textureShift;
	std::string name;
	float hitPoint;

	int index;
	static int nextId;

	sol::table luaMonster;

	void setMonsterData();
	void getMonsterData();

	sol::table scriptTable;
public:
	Monster(Bloodworks *bloodworks, const MonsterTemplate& monsterTemplate);
	~Monster();
	void tick(float dt);
};