#pragma once

#include "cBodyGrid.h"
class Bloodworks;
class GameObject;

class CollisionController
{
	Bloodworks *bloodworks;
	cBodyGrid bodyGrid;

public:

	enum Flags
	{
		no_player_collision = (1 << 0),
		no_monster_collision = (1 << 1),
		no_bullet_collision = (1 << 2),
	};
	CollisionController(Bloodworks *bloodworks);

	void reset();
	void drawDebug(bool drawGrid = true);
	Vec2 getLongestSolver(const Circle& c, unsigned ignoreFlags);

	template<typename T>
	int addCollider(const T& body, GameObject* object)
	{
		return bodyGrid.insertBody(body, object);
	}

	void setFlag(int index, unsigned flag, bool isSet)
	{
		bodyGrid.setFlag(index, flag, isSet);
	}

	void setFlags(int index, unsigned flags)
	{
		bodyGrid.setFlags(index, flags);
	}

	bool hasFlag(int index, unsigned flag)
	{
		return bodyGrid.hasFlag(index, flag);
	}

	unsigned getFlags(int index)
	{
		return bodyGrid.getFlags(index);
	}

	template<typename T>
	void relocateCollider(int id, const T& body)
	{
		bodyGrid.relocateBody(id, body);
	}

	void removeCollider(int id)
	{
		bodyGrid.removeBody(id);
	}

	template<typename T>
	bool hasCollision(const T& body, unsigned ignoreFlags)
	{
		return bodyGrid.hasCollision(body, ignoreFlags);
	}

	Vec2 getFreePosition(float radius, unsigned ignoreFlags );
	float getRayDistance(const Vec2& begin, const Vec2& ray, float radius, unsigned ignoreFlags);
	void updateGrid();
};