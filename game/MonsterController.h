#pragma once

#include <vector>
#include <unordered_map>

class Monster;
class Bloodworks;

#include "cGrid.h"

class MonsterController
{
	std::vector<Monster*> monsters;
	std::unordered_map<int, Monster*> monstersMap;
	Bloodworks *bloodworks;

	cGrid<Monster> grid;
public:
	void init(Bloodworks *bloodworks);
	void tick(float dt);
	void clear();
	const std::vector<Monster*> getMonsterAt(const Vec2& pos);
};