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

	void removeCollider(int id)
	{
		bodyGrid.removeBody(id);
	}
};