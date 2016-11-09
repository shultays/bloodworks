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
		[&](int monsterIndex, const std::string& anim, float startPercentage)
	{
		monstersMap[monsterIndex]->playAnimation(anim, startPercentage);
	});

	grid.init(Vec2(-700, -600), Vec2(1400, 1200), Vec2(50, 50));

	lua.set_function("addMonster",
		[&](std::string monsterTemplate) -> int
	{
		Monster *newMonster = new Monster(this->bloodworks);
		monstersMap[newMonster->index] = newMonster;
		monsters.push_back(newMonster);
		newMonster->init(monsterTemplates[monsterTemplate]);
		grid.insertToGrid(newMonster);
		return newMonster->getId();
	});

	lua.set_function("resetMonster",
		[&](int monsterIndex)
	{
		monstersMap[monsterIndex]->reset();
	});

	lua.set_function("getMonsterCount",
		[&]() -> int
	{
		return (int)monsters.size();
	});

	monsterTemplates["monster"] = new MonsterTemplate("resources/monster/data.json");
}

void MonsterController::tick(float dt)
{
	for (int i = 0; i < monsters.size(); i++)
	{
		if (monsters[i]->isDead)
		{
			grid.removeFromGrid(monsters[i]);
			monstersMap.erase(monsters[i]->getId());
			SAFE_DELETE(monsters[i]);
			monsters[i] = monsters[monsters.size() - 1];
			monsters.resize(monsters.size() - 1);
			i--;
		}
	}

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

	for (auto& monsterTemplate : monsterTemplates)
	{
		auto& m = monsterTemplate.second;
		SAFE_DELETE(m);
	}
	monsterTemplates.clear();
}

const std::vector<Monster*> MonsterController::getMonsterAt(const Vec2& pos)  const
{
	return grid.getNodeAtPos(pos);
}

