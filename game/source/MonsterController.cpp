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
#include "CollisionController.h"
#include "cCircle.h"

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
	if (bloodworks->isFirstTick())
	{
		out << "MonsterController::tick\n";
	}
	ADD_SCOPED_TIME_PROFILER("MonsterController::tick");
	float closestDistance = FLT_MAX;
	Monster* closestMonster = nullptr;
	Vec2 pos = bloodworks->getPlayer()->getCrosshairPos() + bloodworks->getPlayer()->getPosition();
	for (int i = 0; i < monsters.size(); i++)
	{
		auto& monster = monsters[i];
		if (monster->isDead && monster->removeOnDead)
		{
			grid.removeFromGrid(monster);
			monstersMap.erase(monster->getId());
			SAFE_DELETE(monster);
			monsters.swapToTailRemove(i);
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

	if (bloodworks->isFirstTick())
	{
		out << "MonsterController::tick 1\n";
	}
	if (closestMonster)
	{
		closestMonster->setDebug(-1);
	}

	if (bloodworks->isFirstTick())
	{
		out << "MonsterController::tick 2\n";
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
	if (bloodworks->isFirstTick())
	{
		out << "MonsterController::tick end\n";
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

const cVector<Monster*>& MonsterController::getMonsterAt(const Vec2& pos)  const
{
	return grid.getNodeAtPos(pos);
}

bool checkMonster(Monster* monster, Gun* gun, Bullet* bullet, int searchId, cVector<int>& ignoreIds)
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

MonsterController::MonsterHitResult MonsterController::getClosestMonsterOnLine(const Vec2& begin, const Vec2& initialRay, float radius, sol::table& args)
{
	MonsterHitResult result;
	result.distance = initialRay.length();
	result.monster = nullptr;
	Vec2 ray = initialRay;

	unsigned ignoreFlags = args["ignoreFlags"];
	float d = bloodworks->getCollisionController()->getRayDistance(begin, ray, radius, ignoreFlags);
	if (d >= 0.0f)
	{
		result.distance = (d + radius * 2.0f);
		ray = initialRay.normalized() * result.distance;
	}
	else
	{
		result.distance = initialRay.length();
	}

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

bool MonsterController::runForNode(const Vec2& begin, const Vec2& ray, float radius, const IntVec2& index, Gun* gun, Bullet* bullet, int searchId, cVector<int>& ignoreIds, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc)
{
	if (grid.isValidIndex(index) == false)
	{
		return false;
	}
	Vec2 curPos = grid.getStartPos() + grid.getNodeSize() * (index.toVec() + 0.5f);
	if (ignoreFunc && (*ignoreFunc)(curPos))
	{
		return true;
	}

	const cVector<Monster*>& node = grid.getNodeAtIndex(index);
	for (int m = 0; m < node.size(); m++)
	{
		Monster* monster = node[m];
		if (checkMonster(monster, gun, bullet, searchId, ignoreIds) == false)
		{
			continue;
		}

		float distance = cMath::rayCircleIntersection(begin, ray, monster->getPosition(), monster->getRadius() + radius);
		if (distance >= 0.0f)
		{
			if (func(monster))
			{
				return true;
			}
		}
	}

	return false;
}

bool MonsterController::runForRayWithoutCollision(const Vec2& begin, const Vec2& ray, float radius, sol::table& args, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc)
{
	Gun *gun = nullptr;
	Bullet *bullet = nullptr;
	cVector<int> ignoreIds;
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

	int searchId = bloodworks->getUniqueId();

	if (radius > NODE_SIZE)
	{
		for (int m = 0; m < monsters.size(); m++)
		{
			Monster* monster = monsters[m];
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

		bool alwaysHorizontal = abs(ray.y) < 0.0001f;
		bool alwaysVertical = abs(ray.x) < 0.0001f;

		bool checked[3][3];
		memset(checked, 0, sizeof(checked));

		int maxTry = 200;
		bool hasRadius = radius > 0.01f;
		bool finished = false;
		while (maxTry-- > 0 && finished == false)
		{
			if (current == end)
			{
				finished = true;
			}
			if (checked[1][1] == false || hasRadius == false)
			{
				checked[1][1] = true;
				if (runForNode(begin, ray, radius, current, gun, bullet, searchId, ignoreIds, func, ignoreFunc))
				{
					return true;
				}
			}

			Vec2 nextPos = grid.getStartPos() + grid.getNodeSize() * (current + posShift).toVec();
			if (hasRadius)
			{
				Vec2 curPos = grid.getStartPos() + grid.getNodeSize() * (current).toVec();
				Vec2 endPos = grid.getStartPos() + grid.getNodeSize() * (current + 1).toVec();

				if (currentPos.x - radius < curPos.x)
				{
					if (checked[0][1] == false)
					{
						checked[0][1] = true;
						if (runForNode(begin, ray, radius, current + IntVec2(-1, 0), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[0][0] == false && currentPos.y - radius < curPos.y)
					{
						checked[0][0] = true;
						if (runForNode(begin, ray, radius, current + IntVec2(-1, -1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[0][2] == false && currentPos.y + radius > endPos.y)
					{
						checked[0][2] = true;
						if (runForNode(begin, ray, radius, current + IntVec2(-1, +1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
				}

				if (checked[1][0] == false && currentPos.y - radius < curPos.y)
				{
					checked[1][0] = true;
					if (runForNode(begin, ray, radius, current + IntVec2(0, -1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
					{
						return true;
					}
				}
				if (checked[1][2] == false && currentPos.y + radius > endPos.y)
				{
					checked[1][2] = true;
					if (runForNode(begin, ray, radius, current + IntVec2(0, +1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
					{
						return true;
					}
				}

				if (currentPos.x + radius > endPos.x)
				{
					if (checked[2][1] == false)
					{
						checked[2][1] = true;
						if (runForNode(begin, ray, radius, current + IntVec2(+1, 0), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[2][0] == false && currentPos.y - radius < curPos.y)
					{
						checked[2][0] = true;
						if (runForNode(begin, ray, radius, current + IntVec2(+1, -1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
						{
							return true;
						}
					}
					if (checked[2][2] == false && currentPos.y + radius > endPos.y)
					{
						checked[2][2] = true;
						if (runForNode(begin, ray, radius, current + IntVec2(+1, +1), gun, bullet, searchId, ignoreIds, func, ignoreFunc))
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
		if (maxTry <= 0)
		{
			out << "Reached max try on ray cast, error?\n";
		}
	}
	return false;
}


bool MonsterController::runForRadius(const Vec2& pos, float radius, sol::table& args, std::function<bool(Monster*)>& func)
{
	Gun *gun = nullptr;
	Bullet *bullet = nullptr;
	cVector<int> ignoreIds;
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

	int searchId = bloodworks->getUniqueId();

	if (radius < 400.0f)
	{
		Vec2 min = pos - radius;
		Vec2 max = pos + radius;

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
				auto& node = grid.getNodeAtIndex(i, j);
				for (int m = 0; m < node.size(); m++)
				{
					Monster* monster = node[m];
					if (monster->isDead == false && checkMonster(monster, gun, bullet, searchId, ignoreIds))
					{
						float r = radius + monster->getRadius();
						if (monster->getPosition().distanceSquared(pos) < r * r)
						{
							if (func(monster))
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (int m = 0; m < monsters.size(); m++)
		{
			Monster* monster = monsters[m];
			if (monster->isDead == false && checkMonster(monster, gun, bullet, searchId, ignoreIds))
			{
				float r = radius + monster->getRadius();
				if (monster->getPosition().distanceSquared(pos) < r * r)
				{
					if (func(monster))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool MonsterController::runForRay(const Vec2& begin, const Vec2& initialRay, float radius, sol::table& args, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc)
{
	Vec2 ray = initialRay;
	unsigned ignoreFlags = args["ignoreFlags"];
	float d = bloodworks->getCollisionController()->getRayDistance(begin, ray, radius, ignoreFlags);
	if (d >= 0.0f)
	{
		ray = initialRay.normalized() * (d + radius * 2.0f);
	}
	return runForRayWithoutCollision(begin, ray, radius, args, func, ignoreFunc);
}

Monster* MonsterController::getClosestMonster(const Vec2& pos)
{
	static const std::vector<int> temp;
	return getClosestMonsterWithIgnoreId(pos, temp);
}


Monster* MonsterController::getClosestMonsterInRange(const Vec2& pos, float range)
{
	static const std::vector<int> temp;
	return getClosestMonsterInRangeWithIgnoreId(pos, range, temp);
}

Monster* MonsterController::getClosestMonsterInRangeWithIgnoreId(const Vec2& pos, float range, const std::vector<int>& ignoreId)
{
	float closest = range * range;
	Monster *closestMonster = nullptr;

	if (range < 400.0f)
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
				auto& node = grid.getNodeAtIndex(i, j);
				for (int m = 0; m < node.size(); m++)
				{
					Monster* monster = node[m];
					float d;
					if (monster->isDead == false
						&& (i == minIndex.x || i == monster->gridStart.x) && (j == minIndex.y || j == monster->gridStart.y)
						&& (d = monster->getPosition().distanceSquared(pos)) < closest)
					{
						bool valid = true;

						for (int id : ignoreId)
						{
							if (monster->hasIgnoreId(id))
							{
								valid = false;
								break;
							}
						}
						if (valid)
						{
							closest = d;
							closestMonster = monster;
						}
					}
				}
			}
		}
	}
	else
	{
		for (int m=0; m<monsters.size(); m++)
		{
			Monster *monster = monsters[m];
			float d;
			if (monster->isDead == false && closest > (d = monster->getPosition().distanceSquared(pos)))
			{

				bool valid = true;

				for (int id : ignoreId)
				{
					if (monster->hasIgnoreId(id))
					{
						valid = false;
						break;
					}
				}
				if (valid)
				{
					closest = d;
					closestMonster = monster;
				}
			}
		}
	}

	return closestMonster;
}

void MonsterController::getAllMonstersInRange(const Vec2& pos, float range, cVector<Monster*>& foundMonsters)
{
	if (range < 400.0f)
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

				auto& node = grid.getNodeAtIndex(i, j);
				for (int m = 0; m < node.size(); m++)
				{
					Monster* monster = node[m];
					float r = range + monster->getRadius();
					if (monster->isDead == false 
						&& (i == minIndex.x || i == monster->gridStart.x) && (j == minIndex.y || j == monster->gridStart.y)
						&& monster->getPosition().distanceSquared(pos) < r * r)
					{
						foundMonsters.push_back(monster);
					}
				}
			}
		}

	}
	else
	{
		for (int m = 0; m < monsters.size(); m++)
		{
			Monster* monster = monsters[m];
			float r = range + monster->getRadius();
			if (monster->getPosition().distanceSquared(pos) < r * r)
			{
				foundMonsters.push_back(monster);
			}
		}
	}
}

void MonsterController::damageMonstersInRangeWithIgnoreId(const Vec2& pos, float range, int minRange, int maxRange, bool mark, int ignoreId)
{
	cVector<Monster*> found;
	getAllMonstersInRange(pos, range, found);
	for (int m = 0; m < found.size(); m++)
	{
		Monster* monster = found[m];
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
	return monsters.size();
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
	monsterTemplateIndices[t->getScriptName()] = monsterTemplates.size() - 1;
}

Vec2 MonsterController::getRandomPos(sol::table& args)
{
	const AARect& mapLimits = bloodworks->getMapLimits();
	const Vec2& mapMin = mapLimits.getMin();
	const Vec2& mapMax = mapLimits.getMax();

	bool dontCheckCollision = args["dontCheckCollision"];
	bool canGoEdge = args["canBeEdge"];
	bool onEdges = args["onEdges"];
	bool onScreen = args["onScreen"];

	unsigned ignoreFlags = args["ignoreFlags"];

	bool outsideScreen = onScreen == false && ((bool)args["notOnScreen"]);
	bool notNearMonsters = args["notNearMonsters"];
	bool nearPlayer = args["nearPlayer"];
	bool notNearPlayer = nearPlayer == false && (bool)(args["notNearPlayer"]);
	float playerRange = args["playerRange"] ? args["playerRange"].get<float>() : 300.0f;
	
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

		if (dontCheckCollision == false)
		{
			bool hasCollision = bloodworks->getCollisionController()->hasCollision(Circle(pos, 20.0f), ignoreFlags);
			if (hasCollision)
			{
				score += 10000.0f;
			}
		}

		if (notNearMonsters)
		{
			cVector<Monster*> monstersInrange;
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
			auto& node = grid.getNodeAtIndex(IntVec2(i, j));
			for (int m = 0; m < node.size(); m++)
			{
				Monster* monster = node[m];
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
			auto& node = grid.getNodeAtIndex(IntVec2(i, j));
			for(int m=0; m<node.size(); m++)
			{
				Monster* monster = node[m];
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

int MonsterController::onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args)
{
	return customMonsterOnHit(monster, damage, dir, args);
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

Monster* MonsterController::getClosestMonsterWithIgnoreId(const Vec2& pos, const std::vector<int>& ignoreId)
{
	float closest = FLT_MAX;
	Monster *closestMonster = nullptr;
	for (int m = 0; m < monsters.size(); m++)
	{
		Monster *monster = monsters[m];
		float d;
		if (monster->isDead == false && closest > (d = monster->getPosition().distanceSquared(pos)))
		{
			bool valid = true;
			for (int id : ignoreId)
			{
				if (monster->hasIgnoreId(id))
				{
					valid = false;
					break;
				}
			}
			if (valid)
			{
				closest = d;
				closestMonster = monster;
			}
		}
	}

	return closestMonster;
}
