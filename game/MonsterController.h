#pragma once

#include <vector>
#include <unordered_map>

class Monster;
class Bloodworks;
class MonsterTemplate;

#include "cGrid.h"
#include "json.h"

class MonsterController
{
	friend class BloodworksLuaWorld;
	std::vector<Monster*> monsters;
	std::unordered_map<int, Monster*> monstersMap;
	Bloodworks *bloodworks;

	cGrid<Monster> grid;
	std::unordered_map<std::string, MonsterTemplate*> monsterTemplates;

	class MonsterHitResult
	{
	public:
		Monster *monster;
		float distance;
	};
public:
	MonsterController(Bloodworks *bloodworks);
	~MonsterController();
	void tick();
	const std::vector<Monster*>& getMonsterAt(const Vec2& pos) const;

	MonsterHitResult getClosestMonsterOnLine(const Vec2& begin, const Vec2& ray, int ignoreId);

	Monster* getClosestMonster(const Vec2& pos);
	Monster* getClosestMonsterWithIgnoreId(const Vec2& pos, int ignoreId);
	Monster* getClosestMonsterInRange(const Vec2& pos, float range);
	Monster* getClosestMonsterInRangeWithIgnoreId(const Vec2& pos, float range, int ignoreId);
	std::vector<Monster*> getAllMonstersInRange(const Vec2& pos, float range);

	void damageMonstersInRange(const Vec2& pos, float range, int minRange, int maxRange);
	void damageMonstersInRangeWithIgnoreId(const Vec2& pos, float range, int minRange, int maxRange, bool mark, int ignoreId);

	Monster* addMonster(const std::string& monsterTemplateName);
	Monster* getMonsterAtIndex(int index) const;
	int getMonsterCount() const;
	Monster* getMonster(int id) const;

	void addMonsterTemplate(nlohmann::json &j);

	Vec2 getRandomMonsterSpawnPosAux(bool outsideScreen);
	Vec2 getRandomMonsterSpawnPosOutsideScreen();
	Vec2 getRandomMonsterSpawnPos();
};