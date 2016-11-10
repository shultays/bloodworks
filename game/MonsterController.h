#pragma once

#include <vector>
#include <unordered_map>

class Monster;
class Bloodworks;
class MonsterTemplate;

#include "cGrid.h"

class MonsterController
{
	std::vector<Monster*> monsters;
	std::unordered_map<int, Monster*> monstersMap;
	Bloodworks *bloodworks;

	cGrid<Monster> grid;
	std::unordered_map<std::string, MonsterTemplate*> monsterTemplates;
public:
	void init(Bloodworks *bloodworks);
	void tick(float dt);
	void clear();
	const std::vector<Monster*> getMonsterAt(const Vec2& pos) const;

	Monster* getClosestMonster(const Vec2& pos);
	Monster* getClosestMonsterInRange(const Vec2& pos, float range);
	std::vector<Monster*> getAllMonstersInRange(const Vec2& pos, float range);

};