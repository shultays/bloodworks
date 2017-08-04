#pragma once

#include "cBodyGrid.h"
class Bloodworks;
class GameObject;

class CollisionController
{
	Bloodworks *bloodworks;
	cBodyGrid bodyGrid;

public:
	CollisionController(Bloodworks *bloodworks);

	void reset();
	void drawDebug(bool drawGrid = true);
	Vec2 getLongestSolver(const Circle& c);

	template<typename T>
	int addCollider(const T& body, GameObject* object)
	{
		return bodyGrid.insertBody(body, object);
	}

	template<typename T>
	void relocateCollider(int id, const T& body)
	{
		bodyGrid.relocateBody(id, body);
	}

	template<typename T>
	bool hasCollision(const T& body)
	{
		return bodyGrid.hasCollision(body);
	}

	void removeCollider(int id)
	{
		bodyGrid.removeBody(id);
	}

	Vec2 getFreePosition(float radius);

	float getRayDistance(const Vec2& begin, const Vec2& ray, float radius);
};