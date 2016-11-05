#pragma once

#include <vector>

class Monster;
class Bloodworks;

class MonsterController
{
	std::vector<Monster*> monsters;
	Bloodworks *bloodworks;

public:
	void init(Bloodworks *bloodworks);
	void tick(float dt);

	void clear();
};