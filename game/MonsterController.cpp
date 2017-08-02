#include "MonsterController.h"
#include "Bloodworks.h"
#include "Monster.h"
#include "sol.h"
#include "cMath.h"
#include "Player.h"
#include "Bullet.h"
#include "cAnimatedRenderable.h"
#include "MonsterTemplate.h"
#include "Gun.h"
#include "cTimeProfiler.h"

#define NODE_SIZE 50.0f

MonsterController::MonsterController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	AARect boundaries = bloodworks->getMapLimits();
	boundaries.addThreshold(500.0f);

	grid.init(boundaries, Vec2(NODE_SIZE));

	customMonsterTick = lua["customMonsterOnTick"];
	customMonsterOnHit = lua["customMonsterOnHit"];
	customMonsterShouldHit = lua["customMonsterShouldHit"];
	customMonsterOnKill = lua["customMonsterOnKill"];
}

void MonsterController::tick()
{
	ADD_SCOPED_TIME_PROFILER("MonsterController::tick");
	float closestDistance = FLT_MAX;
	Monster* closestMonster = nullptr;
	Vec2 pos = bloodworks->getPlayer()->getCrosshairPos() + bloodworks->getPlayer()->getPosition();
	for (int i = 0; i < monsters.size(); i++)
	{
		auto& monster = monsters[i];
		if (monster->isDead)
		{
			grid.removeFromGrid(monster);
			monstersMap.erase(monster->getId());
			SAFE_DELETE(monster);
			monster = monsters[monsters.size() - 1];
			monsters.resize(monsters.size() - 1);
			i--;
		}
		else
		{
			if (monster->getDebug() == -1)
			{
				monster->setDebug(0);
			}
			if (monster->getDebug() == 0)
			{
				float distance = monster->getPosition().distanceSquared(pos);
				if (closestDistance > distance)
				{
					closestDistance = distance;
					closestMonster = monster;
				}
			}
		}
	}

	if (closestMonster)
	{
		closestMonster->setDebug(-1);
	}

	for (int i=0; i<monsters.size(); i++)
	{
		Monster* monster = monsters[i];
		monster->tick();
		customMonsterTick(monster);
		if (bloodworks->isCoorOutside(monster->position, -20.0f) == false)
		{
			grid.relocate(monster);
		}
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
		SAFE_DELETE(monsterTemplate);
	}
	monsterTemplates.clear();
	monsterTemplateIndices.clear();
}

const std::vector<Monster*>& MonsterController::getMonsterAt(const Vec2& pos)  const
{
	return grid.getNodeAtPos(pos);
}

bool checkMonster(Monster* monster, Gun* gun, Bullet* bullet, int searchId, std::vector<int>& ignoreIds)
{
	if (monster->hasIgnoreId(searchId))
	{
		return false;
	}
	monster->addIgnoreId(searchId);
	for (int ignoreId : ignoreIds)
	{
		if (monster->hasIgnoreId(ignoreId))
		{
			return false;
		}
	}
	if (bullet)
	{
		if (monster->hasIgnoreId(bullet->getId()) || monster->shouldHit(bullet) == false)
		{
			return false;
		}
	}
	if (gun)
	{
		if (monster->shouldHit(gun) == false)
		{
			return false;
		}
	}

	return true;
}

MonsterController::MonsterHitResult MonsterController::getClosestMonsterOnLine(const Vec2& begin, const Vec2& ray, float radius, sol::table& args)
{
	MonsterHitResult result;
	result.distance = 100000000.0f;
	result.monster = nullptr;

	std::function<bool(Monster*)> func = [&](Monster* monster)
	{
		float distance = cMath::rayCircleIntersection(begin, ray, monster->getPosition(), monster->getRadius() + radius);
		if (distance >= 0.0f && distance < result.distance)
		{
			result.distance = distance;
			result.monster = monster;
		}
		return false;
	};

	std::function<bool(const Vec2&)> func2 = [&](const Vec2& pos)
	{
		float maxDistance = result.distance + NODE_SIZE * 2.0f + radius;
		if (pos.distanceSquared(begin) > maxDistance * maxDistance)
		{
			return true;
		}
		return false;
	};

	runForRay(begin, ray, radius, args, func, &func2);

	return result;
}

bool MonsterController::runForNode(const IntVec2& index, Gun* gun, Bullet* bullet, int searchId, std::vector<int>& ignoreIds, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc)
{
	Vec2 curPos = grid.getStartPos() + grid.getNodeSize() * (index.toVec() + 0.5f);
	if (ignoreFunc && (*ignoreFunc)(curPos))
	{
		return true;
	}
	bool renderDebug = input.isKeyDown(key_f);
	if (renderDebug)
	{
		grid.drawDebug(index, 0xFF0000FF);
	}

	const std::vector<Monster*> node = grid.getNodeAtIndex(index);
	for (Monster* monster : node)
	{
		if (checkMonster(monster, gun, bullet, searchId, ignoreIds) == false)
		{
			continue;
		}
		if (func(monster))
		{
			return true;
		}
	}

	return false;
}

bool MonsterController::runForRay(const Vec2& begin, const Vec2& ray, float radius, sol::table& args, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc)
{
	Gun *gun = nullptr;
	Bullet *bullet = nullptr;
	std::vector<int> ignoreIds;
	if (args)
	{
		gun = args["gun"];
		bullet = args["bullet"];

		if (auto argIgnoreIds = args["ignoreIds"])
		{
			int t = 1;
			while (argIgnoreIds[t])
			{
				ignoreIds.push_back(argIgnoreIds[t].get<int>());
				t++;
			}
		}
	}

	bool renderDebug = input.isKeyDown(key_f);

	int searchId = bloodworks->getUniqueId();

	if (radius > NODE_SIZE)
	{
		for (Monster* monster : monsters)
		{
			if (checkMonster(monster, gun, bullet, searchId, ignoreIds) == false)
			{
				continue;
			}
			monster->addIgnoreId(searchId);
			if (func(monster))
			{
				return true;
			}
		}
	}
	else
	{
		IntVec2 start = grid.getNodeIndex(begin);
		IntVec2 end = grid.getNodeIndex(begin + ray);

		IntVec2 dirInt(ray.x > 0.0f ? 1 : -1, ray.y > 0.0f ? 1 : -1);
		IntVec2 posShift(ray.x > 0.0f ? 1 : 0, ray.y > 0.0f ? 1 : 0);

		Vec2 currentPos = begin;
		IntVec2 current = start;

		int checkVal = abs(ray.x) < abs(ray.y) ? 1 : 0;

		int maxText = 20;
		bool alwaysHorizontal = abs(ray.y) < 0.0001f;
		bool alwaysVertical = abs(ray.x) < 0.0001f;

		bool checked[3][3];
		memset(checked, 0, sizeof(checked));


		bool hasRadius = radius > 0.01f;
		while(maxText --> 0)
		{
			if (checked[1][1] == false || hasRadius == false)
			{
				checked[1][1] = true;
				if (runForNode(current, gun, bullet, searchId, ignoreIds, func, ignoreFunc))
				{
					return true;
				}
			}
		
			Vec2 nextPos = grid.getStartPos() + grid.getNodeSize() * (current + posShift).toVec();
			if (renderDebug)
			{
				debugRenderer.addCircle(currentPos, 3.0f, 0.0f, 0xFF00FFFF);
			}
			if (hasRadius)
			{
				Vec2 curPos = grid.getStartPos() + grid.getNodeSize() * (current).toVec();
				Vec2 endPos = grid.getStartPos() + grid.getNodeSize() * (current + 1).toVec();

				if (currentPos.x - radius < curPos.x)
				{
					if (checked[0][1] == false)
					{
						checked[0][1] = true;
						if (runForNode(current + IntVec2(-1, 0), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[0][0] == false && currentPos.y - radius < curPos.y)
					{
						checked[0][0] = true;
						if (runForNode(current + IntVec2(-1, -1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[0][2] == false && currentPos.y + radius > endPos.y)
					{
						checked[0][2] = true;
						if(runForNode(current + IntVec2(-1, +1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
				}

				if (checked[1][0] == false && currentPos.y - radius < curPos.y)
				{
					checked[1][0] = true;
					if (runForNode(current + IntVec2(0, -1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
					{
						return true;
					}
				}
				if (checked[1][2] == false && currentPos.y + radius > endPos.y)
				{
					checked[1][2] = true;
					if (runForNode(current + IntVec2(0, +1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
					{
						return true;
					}
				}

				if (currentPos.x + radius > endPos.x)
				{
					if (checked[2][1] == false)
					{
						checked[2][1] = true;
						if(runForNode(current + IntVec2(+1, 0), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[2][0] == false && currentPos.y - radius < curPos.y)
					{
						checked[2][0] = true;
						if(runForNode(current + IntVec2(+1, -1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[2][2] == false && currentPos.y + radius > endPos.y)
					{
						checked[2][2] = true;
						if(runForNode(current + IntVec2(+1, +1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
				}
			}


			Vec2 d = (nextPos - begin) / ray;
			if (d[0] > 1.0f && d[1] > 1.0f)
			{
				break;
			}

			bool v = false;

			if (alwaysHorizontal)
			{
				current.x += dirInt.x;
				currentPos.x = nextPos.x;
				currentPos.y = begin.y + ((currentPos.x - begin.x) / ray.x) * ray.y;
			}
			else if (alwaysVertical)
			{
				current.y += dirInt.y;
				currentPos.y = nextPos.y;
				currentPos.x = begin.x + ((currentPos.y - begin.y) / ray.y) * ray.x;
				v = true;
			}
			else
			{
				Vec2 diff = (nextPos - currentPos);
				float dx = diff.x / ray.x;
				float dy = diff.y / ray.y;
				if (dx < dy)
				{
					current.x += dirInt.x;
					currentPos.x = nextPos.x;
					currentPos.y = begin.y + ((currentPos.x - begin.x) / ray.x) * ray.y;
				}
				else
				{
					current.y += dirInt.y;
					currentPos.y = nextPos.y;
					currentPos.x = begin.x + ((currentPos.y - begin.y) / ray.y) * ray.x;
					v = true;
				}
			}

			if (hasRadius)
			{
				if (v)
				{
					if (dirInt.y < 0)
					{
						for (int i = 0; i < 3; i++)
						{
							checked[i][2] = checked[i][1];
							checked[i][1] = checked[i][0];
							checked[i][0] = false;
						}
					}
					else
					{
						for (int i = 0; i < 3; i++)
						{
							checked[i][0] = checked[i][1];
							checked[i][1] = checked[i][2];
							checked[i][2] = false;
						}
					}
				}
				else
				{
					if (dirInt.x < 0)
					{
						for (int i = 0; i < 3; i++)
						{
							checked[2][i] = checked[1][i];
							checked[1][i] = checked[0][i];
							checked[0][i] = false;
						}
					}
					else
					{
						for (int i = 0; i < 3; i++)
						{
							checked[0][i] = checked[1][i];
							checked[1][i] = checked[2][i];
							checked[2][i] = false;
						}
					}
				}
			}
		}
		if (renderDebug)
		{
			Vec2 perp = ray.normalized().sideVec();
			//debugRenderer.addLine(begin, begin + ray, 0.0f, 0xFFFF0000);
			debugRenderer.addCircle(begin, radius, 0.0f, 0xFFFF0000);
			debugRenderer.addCircle(begin + ray, radius, 0.0f, 0xFFFF0000);
			debugRenderer.addLine(begin + perp * radius, begin + perp * radius + ray, 0.0f, 0xFFFF0000);
			debugRenderer.addLine(begin - perp * radius, begin - perp * radius + ray, 0.0f, 0xFFFF0000);
		}
	}
	return false;
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

void MonsterController::getAllMonstersInRange(const Vec2& pos, float range, std::vector<Monster*>& foundMonsters)
{
	float maxDistanceSquared = range * range;

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
		for (Monster* monster : monsters)
		{
			if (monster->getPosition().distanceSquared(pos) < maxDistanceSquared)
			{
				foundMonsters.push_back(monster);
			}
		}
	}
}

void MonsterController::damageMonstersInRangeWithIgnoreId(const Vec2& pos, float range, int minRange, int maxRange, bool mark, int ignoreId)
{
	std::vector<Monster*> found;
	getAllMonstersInRange(pos, range, found);
	for (Monster* monster : found)
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
	MonsterTemplate *monsterTemplate = monsterTemplates[monsterTemplateIndices[monsterTemplateName]];

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

void MonsterController::addMonsterTemplate(nlohmann::json& j, const DirentHelper::File& file)
{
	MonsterTemplate *t = new MonsterTemplate(j, file);
	monsterTemplates.push_back(t);
	monsterTemplateIndices[t->getScriptName()] = (int)monsterTemplates.size() - 1;
}

Vec2 MonsterController::getRandomPos(sol::table& args)
{
	const AARect& mapLimits = bloodworks->getMapLimits();
	const Vec2& mapMin = mapLimits.getMin();
	const Vec2& mapMax = mapLimits.getMax();

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

	AARect screenRect = bloodworks->getScreenRect();
	screenRect.addThreshold(-50.0f);
	while (tryCount --> 0)
	{
		float score = 0.0f;
		Vec2 pos;
		if (onScreen)
		{
			pos = screenRect.getRandomPos();
		}
		else if (nearPlayer)
		{
			pos = bloodworks->getPlayer()->getPosition() + Vec2::fromAngle(randFloat(pi_2)) * playerRange * sqrtf(randFloat());
		}
		else if (onEdges || (canGoEdge && randFloat() < 0.2f))
		{
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
			std::vector<Monster*> monstersInrange;
			getAllMonstersInRange(pos, 100.0f, monstersInrange);
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
			if (pos.x > screenRect.getMin().x && pos.x < screenRect.getMax().x && pos.y > screenRect.getMin().y && pos.y < screenRect.getMax().y)
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

void MonsterController::runForEachMonsterInRadius(const Vec2& pos, float radius, std::function<bool(Monster *monster) >& func) const
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
					float r = radius + monster->getRadius();	
					float rSquared = r * r;
					if (monster->getPosition().distanceSquared(pos) < rSquared && func(monster) == false)
					{
						return;
					}
				}
			}
		}
	}
}

void MonsterController::runForEachMonsterInPie(const Vec2& pos, float radius, float angle, float angleWidth, std::function<bool(Monster *monster) >& func) const
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
					float r = radius + monster->getRadius();
					float rSquared = r * r;
					if (monster->getPosition().distanceSquared(pos) < rSquared)
					{
						Vec2 diff = monster->getPosition() - pos;
						float d = diff.length() + 0.0001f;
						float toMonster = diff.toAngle();
						float diffAngle = angleDiff(angle, toMonster);
						float addAngle = atan2(monster->getRadius(), d);
						if (abs(diffAngle) < angleWidth + addAngle)
						{
							if (func(monster) == false)
							{
								return;
							}
						}
					}
				}
			}
		}
	}
}

void MonsterController::relocateMonster(Monster* monster)
{
	grid.relocate(monster);
}

void MonsterController::drawDebug()
{
	grid.drawDebug();
}

void MonsterController::onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args)
{
	customMonsterOnHit(monster, damage, dir, args);
}

bool MonsterController::shouldHit(Monster* monster, Gun* gun, Bullet *bullet)
{
	return customMonsterShouldHit(monster, gun, bullet);
}

void MonsterController::onMonsterDied(Monster* monster)
{
	customMonsterOnKill(monster);
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
