#pragma once

#include <vector>
#include <unordered_map>

class Monster;
class Bloodworks;
class MonsterTemplate;
class Gun;
class Bullet;

#include "cGrid.h"
#include "sol.h"
#include "json.h"
#include "DirentHelper.h"

class MonsterController
{
	friend class BloodworksLuaWorld;
	cVector<Monster*> monsters;
	std::unordered_map<int, Monster*> monstersMap;
	Bloodworks *bloodworks;

	cGrid<Monster> grid;

	cVector<MonsterTemplate*> monsterTemplates;
	std::unordered_map<std::string, int> monsterTemplateIndices;

	sol::function customMonsterTick;
	sol::function customMonsterOnHit;
	sol::function customMonsterOnKill;
	sol::function customMonsterShouldHit;

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
	const cVector<Monster*>& getMonsterAt(const Vec2& pos) const;

	MonsterHitResult getClosestMonsterOnLine(const Vec2& begin, const Vec2& ray, float radius, sol::table& args);
	bool runForRayWithoutCollision(const Vec2& begin, const Vec2& ray, float radius, sol::table& args, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc = nullptr);
	bool runForRay(const Vec2& begin, const Vec2& ray, float radius, sol::table& args, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc = nullptr);
	bool runForNode(const Vec2& begin, const Vec2& ray, float radius, const IntVec2& index, Gun* gun, Bullet* bullet, int searchId, cVector<int>& ignoreIds, std::function<bool(Monster*)>& func, std::function<bool(const Vec2&)>* ignoreFunc);

	Monster* getClosestMonster(const Vec2& pos);
	Monster* getClosestMonsterWithIgnoreId(const Vec2& pos, const std::vector<int>& ignoreId);
	Monster* getClosestMonsterInRange(const Vec2& pos, float range);
	Monster* getClosestMonsterInRangeWithIgnoreId(const Vec2& pos, float range, const std::vector<int>& ignoreId);
	void getAllMonstersInRange(const Vec2& pos, float range, cVector<Monster*>& foundMonsters);

	void damageMonstersInRange(const Vec2& pos, float range, int minRange, int maxRange);
	void damageMonstersInRangeWithIgnoreId(const Vec2& pos, float range, int minRange, int maxRange, bool mark, int ignoreId);

	Monster* addMonster(const std::string& monsterTemplateName);
	Monster* getMonsterAtIndex(int index) const;
	int getMonsterCount() const;
	Monster* getMonster(int id) const;

	void addMonsterTemplate(nlohmann::json& j, const DirentHelper::File& file);
	Vec2 getRandomPos(sol::table& args);
	void reset();
	void runForEachMonsterInRadius(const Vec2& pos, float radius, std::function<bool(Monster *monster) >& func) const;
	void runForEachMonsterInPie(const Vec2& pos, float radius, float angle, float angleWidth, std::function<bool(Monster *monster) >& func) const;
	void relocateMonster(Monster* monster);

	const cVector<MonsterTemplate*>& getTemplates() const
	{
		return monsterTemplates;
	}
	void drawDebug();
	void onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args);
	bool shouldHit(Monster* monster, Gun* gun, Bullet *bullet);
	void onMonsterDied(Monster* monster);
};