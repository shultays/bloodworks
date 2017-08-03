#pragma once

#include "cBodyGrid.h"
class Bloodworks;

class CollisionController
{
	Bloodworks *bloodworks;
	cBodyGrid bodyGrid;

public:
	CollisionController(Bloodworks *bloodworks);
	void drawDebug(bool drawGrid = true);
	Vec2 getLongestSolver(const Circle& c);
};