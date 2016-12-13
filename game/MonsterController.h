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
	std::vector<Monster*> monsters;
	std::unordered_map<int, Monster*> monstersMap;
	Bloodworks *bloodworks;

	cGrid<Monster> grid;
	std::unordered_map<std::string, MonsterTemplate*> monsterTemplates;
public:
	MonsterController(Bloodworks *bloodworks);
	~MonsterController();
	void tick();
	const std::vector<Monster*>& getMonsterAt(const Vec2& pos) const;

	Monster* getClosestMonster(const Vec2& pos);
	Monster* getClosestMonsterWithIgnoreData(const Vec2& pos, const std::string& ignoreData);
	Monster* getClosestMonsterInRange(const Vec2& pos, float range);
	Monster* getClosestMonsterInRangeWithIgnoreData(const Vec2& pos, float range, const std::string& ignoreData);
	std::vector<Monster*> getAllMonstersInRange(const Vec2& pos, float range);

	void damageMonstersInRange(const Vec2& pos, float range, int minRange, int maxRange);
	void damageMonstersInRangeWithIgnoreData(const Vec2& pos, float range, int minRange, int maxRange, bool mark, const std::string& ignoreData);

	Monster* addMonster(const std::string& monsterTemplateName);
	int getMonsterCount() const;
	Monster* getMonster(int id) const;

	void addMonsterTemplate(nlohmann::json &j);
};