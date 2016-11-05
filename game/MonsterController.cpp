#include "MonsterController.h"
#include "Bloodworks.h"
#include "Monster.h"

void MonsterController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	MonsterTemplate monsterTemplate("resources/monster/data.json");
	for (int i = 0; i < 10; i++)
	{
		monsters.push_back(new Monster(bloodworks, monsterTemplate));
	}

}

void MonsterController::tick(float dt)
{
	lua["dt"] = dt;
	lua["time"] = timer.getTime();
	for (auto& monster : monsters)
	{
		monster->tick(dt);
	}
}

void MonsterController::clear()
{
	for (auto& monster : monsters)
	{
		SAFE_DELETE(monster);
	}
}

