#pragma once

#include "cGlobals.h"

class Gun;
class Bonus;
class cTexturedQuadRenderable;
class Bloodworks;
class Monster;

#define DEFAULT_DROP_DURATION 45.0f

class DropController
{
	Bloodworks *bloodworks;
	struct Drop
	{
		Gun *gun;
		Bonus *bonus;
		Vec2 pos;
		cTexturedQuadRenderable *renderable;
		cTextRenderable *text;
		float time;
		Vec2 gunTextureSize;

		bool canFadeout;
		float spawnTime;
		int id;
	};

	cVector<Drop> drops;

	float lastDropSpawn;
	float lastRandomDropSpawn;

	float dropSpawnInterval;
	float monsterDropSpawnInterval;
public:
	DropController(Bloodworks *bloodworks)
	{
		this->bloodworks = bloodworks;
		lastDropSpawn = 0.0f;
		lastRandomDropSpawn = 0.0f;
	}
	~DropController();

	int spawnDrop(const Vec2& position, float spawnTime = DEFAULT_DROP_DURATION);
	int spawnGun(const Vec2& position, int forceIndex = -1, float spawnTime = DEFAULT_DROP_DURATION);
	int spawnBonus(const Vec2& position, int forceIndex = -1, float spawnTime = DEFAULT_DROP_DURATION);

	void tick();
	void reset();
	void onMonsterDied(Monster* monster, float dropChance);
	float getLastSpawnTime() const;
	void clearButHighlighted();
	void removeDrop(int id);
	void setMonsterDropInterval(float interval);
	void setDropInterval(float interval);
};