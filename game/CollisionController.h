#pragma once

#include "cBodyGrid.h"
class Bloodworks;

class CollisionController
{
	Bloodworks *bloodworks;
	cBodyGrid bodyGrid;

public:
	CollisionController(Bloodworks *bloodworks);

};