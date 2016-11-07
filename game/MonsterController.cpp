#include "MonsterController.h"
#include "Bloodworks.h"
#include "Monster.h"

void MonsterController::init(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	lua.set_function("addTimer",
		[&](int monsterIndex, float time, const std::string& func, sol::table table)
	{
		monstersMap[monsterIndex]->addTimer(time, func, table);
	});

	lua.set_function("playAnimation",
		[&](int monsterIndex, const std::string& anim)
	{
		monstersMap[monsterIndex]->playAnimation(anim);
	});

	grid.init(Vec2(-500, -400), Vec2(1000, 800), Vec2(50, 50));

	MonsterTemplate monsterTemplate("resources/monster/data.json");
	for (int i = 0; i < 10; i++)
	{
		Monster *newMonster = new Monster(bloodworks);
		monstersMap[newMonster->index] = newMonster;
		newMonster->init(monsterTemplate);
		monsters.push_back(newMonster);
		grid.insertToGrid(newMonster);
	}
}

void MonsterController::tick(float dt)
{
	for (auto& monster : monsters)
	{
		monster->tick(dt);
		grid.relocate(monster);
	}

	if (input.isKeyDown(key_f1))
	{
		grid.drawDebug();
	}
}

void MonsterController::clear()
{
	for (auto& monster : monsters)
	{
		grid.removeFromGrid(monster);
		SAFE_DELETE(monster);
	}
}

const std::vector<Monster*> MonsterController::getMonsterAt(const Vec2& pos)  const
{
	return grid.getNodeAtPos(pos);
}

