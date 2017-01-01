#include "MonsterController.h"
#include "Bloodworks.h"
#include "Monster.h"
#include "sol.h"
#include "cMath.h"

#include "Player.h"


MonsterController::MonsterController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	grid.init(bloodworks->getMapMin() - 50.0f, bloodworks->getMapSize() + 100.0f, Vec2(50.0f));
}

void MonsterController::tick()
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
		monster->tick();
		if (bloodworks->isCoorOutside(monster->position) == false)
		{
			grid.relocate(monster);
		}
	}
	
	if (input.isKeyDown(key_f1))
	{
		grid.drawDebug();
	}
}

MonsterController::~MonsterController()
{
	for (auto& monster : monsters)
	{
		grid.removeFromGrid(monster);
		SAFE_DELETE(monster);
	}
	monsters.clear();
	for (auto& monsterTemplate : monsterTemplates)
	{
		auto& m = monsterTemplate.second;
		SAFE_DELETE(m);
	}
	monsterTemplates.clear();
}

const std::vector<Monster*>& MonsterController::getMonsterAt(const Vec2& pos)  const
{
	return grid.getNodeAtPos(pos);
}

MonsterController::MonsterHitResult MonsterController::getClosestMonsterOnLine(const Vec2& begin, const Vec2& ray, int ignoreId)
{
	MonsterHitResult result;
	result.distance = FLT_MAX;
	result.monster = nullptr;

	if (input.isKeyDown(key_f3))
	{
		debugRenderer.addLine(begin, begin + ray);
		for (auto& monster : monsters)
		{
			float t;
			if ((t = cMath::rayCircleIntersection(begin, ray, monster->getPosition(), monster->getRadius())) >= 0.0f)
			{
				debugRenderer.addCircle(monster->getPosition(), monster->getRadius(), 0.0f, Vec4(1.0f, 0.0f, 0.0f, 1.0f));
				debugRenderer.addCircle(begin + ray.normalized() * t, 5, 0.0f, Vec4(1.0f, 0.0f, 0.0f, 1.0f));
			}
			else
			{
				debugRenderer.addCircle(monster->getPosition(), monster->getRadius(), 0.0f, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
	}

	for (auto & monster : monsters)
	{
		float distance = cMath::rayCircleIntersection(begin, ray, monster->getPosition(), monster->getRadius());
		if (distance >= 0.0f && distance < result.distance)
		{
			result.distance = distance;
			result.monster = monster;
		}
	}

	return result;
}

Monster* MonsterController::getClosestMonster(const Vec2& pos)
{
	return getClosestMonsterWithIgnoreId(pos, -1);
}


Monster* MonsterController::getClosestMonsterInRange(const Vec2& pos, float range)
{
	return getClosestMonsterInRangeWithIgnoreId(pos, range, -1);
}

Monster* MonsterController::getClosestMonsterInRangeWithIgnoreId(const Vec2& pos, float range, int ignoreId)
{
	float closest = range * range;
	Monster *closestMonster = nullptr;

	if (range > 200)
	{
		Vec2 min = pos - range;
		Vec2 max = pos + range;

		IntVec2 minIndex = grid.getNodeIndex(min);
		IntVec2 maxIndex = grid.getNodeIndex(max);

		if (minIndex.x < 0)
		{
			minIndex.x = 0;
		}
		if (minIndex.y < 0)
		{
			minIndex.y = 0;
		}

		if (maxIndex.x > grid.getNodeCount().x - 1)
		{
			maxIndex.x = grid.getNodeCount().x - 1;
		}
		if (maxIndex.y < grid.getNodeCount().y - 1)
		{
			maxIndex.y = grid.getNodeCount().y - 1;
		}

		for (int i = minIndex.x; i <= maxIndex.x; i++)
		{
			for (int j = minIndex.y; j <= maxIndex.y; j++)
			{
				for (auto& monster : grid.getNodeAtIndex(i, j))
				{
					float d;
					if (monster->isDead == false
						&& (i == minIndex.x || i == monster->gridStart.x) && (j == minIndex.y || j == monster->gridStart.y)
						&& (d = monster->getPosition().distanceSquared(pos)) < closest
						&& (ignoreId == -1 || monster->hasIgnoreId(ignoreId) == false))
					{
						closest = d;
						closestMonster = monster;
					}
				}
			}
		}
	}
	else
	{
		for (auto& monster : monsters)
		{
			float d;
			if (monster->isDead == false && closest > (d = monster->getPosition().distanceSquared(pos)) && (ignoreId == -1 ||  monster->hasIgnoreId(ignoreId) == false))
			{
				closest = d;
				closestMonster = monster;
			}
		}
	}

	return closestMonster;
}

std::vector<Monster*> MonsterController::getAllMonstersInRange(const Vec2& pos, float range)
{
	float maxDistanceSquared = range * range;

	std::vector<Monster*> foundMonsters;

	if (range > 200)
	{
		Vec2 min = pos - range;
		Vec2 max = pos + range;

		IntVec2 minIndex = grid.getNodeIndex(min);
		IntVec2 maxIndex = grid.getNodeIndex(max);

		if (minIndex.x < 0)
		{
			minIndex.x = 0;
		}
		if (minIndex.y < 0)
		{
			minIndex.y = 0;
		}

		if (maxIndex.x > grid.getNodeCount().x - 1)
		{
			maxIndex.x = grid.getNodeCount().x - 1;
		}
		if (maxIndex.y < grid.getNodeCount().y - 1)
		{
			maxIndex.y = grid.getNodeCount().y - 1;
		}

		for (int i = minIndex.x; i <= maxIndex.x; i++)
		{
			for (int j = minIndex.y; j <= maxIndex.y; j++)
			{
				IntVec2 index(i, j);
				for (auto& monster : grid.getNodeAtIndex(i, j))
				{
					if (monster->isDead == false 
						&& (i == minIndex.x || i == monster->gridStart.x) && (j == minIndex.y || j == monster->gridStart.y)
						&& monster->getPosition().distanceSquared(pos) < maxDistanceSquared)
					{
						foundMonsters.push_back(monster);
					}
				}
			}
		}

	}
	else
	{
		for (auto& monster : monsters)
		{
			if (monster->getPosition().distanceSquared(pos) < maxDistanceSquared)
			{
				foundMonsters.push_back(monster);
			}
		}
	}

	return foundMonsters;
}

void MonsterController::damageMonstersInRangeWithIgnoreId(const Vec2& pos, float range, int minRange, int maxRange, bool mark, int ignoreId)
{
	std::vector<Monster*> monsters = getAllMonstersInRange(pos, range);
	for (auto& monster : monsters)
	{
		if (ignoreId  == -1|| monster->hasIgnoreId(ignoreId) == false)
		{
			monster->doDamage(randInt(minRange, maxRange), (monster->position - pos).normalized());
			if (mark)
			{
				monster->addIgnoreId(ignoreId);
			}
		}
	}
}

Monster* MonsterController::addMonster(const std::string& monsterTemplateName)
{
	MonsterTemplate *monsterTemplate = monsterTemplates[monsterTemplateName];

	Monster *newMonster = new Monster(bloodworks);
	monstersMap[newMonster->id] = newMonster;
	monsters.push_back(newMonster);
	newMonster->init(monsterTemplate);
	grid.insertToGrid(newMonster);
	return newMonster;
}

int MonsterController::getMonsterCount() const
{
	return (int)monsters.size();
}

Monster* MonsterController::getMonsterAtIndex(int index) const
{
	return monsters[index];
}

Monster* MonsterController::getMonster(int id) const
{
	auto& element = monstersMap.find(id);
	return element == monstersMap.end() ? nullptr : element->second;
}

void MonsterController::addMonsterTemplate(nlohmann::json &j)
{
	MonsterTemplate *t = new MonsterTemplate(j);
	monsterTemplates[t->getName()] = t;
}

Vec2 MonsterController::getRandomPos(sol::table& args)
{
	const Vec2& mapMin = bloodworks->getMapMin();
	const Vec2& mapMax = bloodworks->getMapMax();

	bool canGoEdge = args["canBeEdge"];
	bool onEdges = args["onEdges"];
	bool onScreen = args["onScreen"];

	bool outsideScreen = onScreen == false && ((bool)args["notOnScreen"]);
	bool notNearMonsters = args["notNearMonsters"];
	bool nearPlayer = args["nearPlayer"];
	bool notNearPlayer = nearPlayer == false && (bool)(args["notNearPlayer"]);
	float playerRange = args["playerRange"] ? args["playerRange"].get<float>() : 200.0f;
	
	float bestScore = FLT_MAX;
	Vec2 bestPos;
	int tryCount = outsideScreen ? 20 : 10;

	Vec2 screenMin = bloodworks->getCameraPos() - bloodworks->getScreenDimensions().toVec() * 0.5f * bloodworks->getCameraZoom() - 50;
	Vec2 screenMax = bloodworks->getCameraPos() + bloodworks->getScreenDimensions().toVec() * 0.5f * bloodworks->getCameraZoom() + 50;

	while (tryCount --> 0)
	{
		float score = 0.0f;
		Vec2 pos;
		if (onScreen)
		{
			pos = Vec2(randFloat(screenMin.x, screenMax.x), randFloat(screenMin.y, screenMax.y));
		}
		else if (nearPlayer)
		{
			pos = bloodworks->getPlayer()->getPosition() + Vec2::fromAngle(randFloat(pi_2)) * playerRange * sqrtf(randFloat());
		}
		else if (onEdges || (canGoEdge && randFloat() < 0.2f))
		{
			const Vec2& mapMin = bloodworks->getMapMin();
			const Vec2& mapMax = bloodworks->getMapMax();
			int r = randInt(4);
			switch (r)
			{
			case 0:
				return Vec2(mapMin.x - randFloat(50.0f, 150.0f), randFloat(mapMin.y, mapMax.y));
			case 1:
				return Vec2(mapMax.x + randFloat(50.0f, 150.0f), randFloat(mapMin.y, mapMax.y));
			case 2:
				return Vec2(randFloat(mapMin.x - 150.0f, mapMax.x + 150.0f), mapMin.y - randFloat(50.0f, 150.0f));
			case 3:
				return Vec2(randFloat(mapMin.x - 150.0f, mapMax.x + 150.0f), mapMax.y + randFloat(50.0f, 150.0f));
			}
		}
		else
		{
			pos = Vec2(randFloat(mapMin.x + 50.0f, mapMax.x - 50.0f), randFloat(mapMin.y + 50.0f, mapMax.y - 50.0f));
		}

		if (notNearMonsters)
		{
			auto monstersInrange = getAllMonstersInRange(pos, 100.0f);
			for (auto& m : monstersInrange)
			{
				score += 100.0f * 100.0f - m->getPosition().distanceSquared(pos);
			}
		}

		if (notNearPlayer)
		{
			float distanceSquaredToPlayer = bloodworks->getPlayer()->getPosition().distanceSquared(pos);
			if (distanceSquaredToPlayer < playerRange * playerRange)
			{
				score += 200.0f;
				score += (playerRange * playerRange - distanceSquaredToPlayer) * 20.0f;
			}
		}

		if (outsideScreen)
		{
			if (pos.x > screenMin.x && pos.x < screenMax.x && pos.y > screenMin.y && pos.y < screenMax.y)
			{
				score += 100.0f * 100.0f;
			}
		}

		if (score <= 0.1f)
		{
			return pos;
		}
		else if (score < bestScore)
		{
			bestScore = score;
			bestPos = pos;
		}
	}

	return bestPos;
}

void MonsterController::reset()
{
	for (auto& monster : monsters)
	{
		grid.removeFromGrid(monster);
		SAFE_DELETE(monster);
	}
	monsters.clear();
}

void MonsterController::runForEachMonsterInRadius(Vec2 pos, float radius, std::function<bool(Monster *monster) >& func) const
{
	IntVec2 start = grid.getNodeIndex(pos - radius);
	IntVec2 end = grid.getNodeIndex(pos + radius);
	int runIndex = bloodworks->getUniqueId();
	for (int i = start.x; i <= end.x; i++)
	{
		for (int j = start.y; j <= end.y; j++)
		{
			auto node = grid.getNodeAtIndex(IntVec2(i, j));
			for (Monster *monster : node)
			{
				if (monster->lastRunCheck != runIndex)
				{
					monster->lastRunCheck = runIndex;
					if (func(monster) == false)
					{
						return;
					}
				}
			}
		}
	}
}

void MonsterController::damageMonstersInRange(const Vec2& pos, float range, int minRange, int maxRange)
{
	damageMonstersInRangeWithIgnoreId(pos, range, minRange, maxRange, false, -1);
}

Monster* MonsterController::getClosestMonsterWithIgnoreId(const Vec2& pos, int ignoreId)
{
	float closest = FLT_MAX;
	Monster *closestMonster = nullptr;
	for (auto& monster : monsters)
	{
		float d;
		if (monster->isDead == false && closest > (d = monster->getPosition().distanceSquared(pos)) && (ignoreId == -1 || monster->hasIgnoreId(ignoreId) == false))
		{
			closest = d;
			closestMonster = monster;
		}
	}

	return closestMonster;
}
