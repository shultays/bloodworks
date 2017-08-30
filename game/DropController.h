#pragma once

#include "cGlobals.h"

class Gun;
class Bonus;
class cTexturedQuadRenderable;
class Bloodworks;
class Monster;

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

		bool canFadeout;
	};

	cVector<Drop> drops;

	float lastDropSpawn;
	float lastRandomDropSpawn;
public:
	DropController(Bloodworks *bloodworks)
	{
		this->bloodworks = bloodworks;
		lastDropSpawn = 0.0f;
		lastRandomDropSpawn = 0.0f;
	}
	~DropController();

	void spawnGun(const Vec2& position, int forceIndex = -1);
	void spawnBonus(const Vec2& position, int forceIndex = -1);
	void tick();
	void spawnDrop(const Vec2& position);
	void reset();
	void onMonsterDied(Monster* monster, float dropChance);
	float getLastSpawnTime() const;
};