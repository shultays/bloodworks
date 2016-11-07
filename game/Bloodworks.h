#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
#include "MonsterController.h"
#include "BulletController.h"
#include "MissionController.h"

class cTexturedQuadRenderable;
class Player;
class Monster;
class Bullet;
class Gun;
class MissionController;

class Bloodworks : public cGame
{
	cTexturedQuadRenderable *bg;
	Player *player;


	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;

	MissionController missionController;
	MonsterController monsterController;
	BulletController bulletController;
	Gun *gun;
protected:
	virtual void render() override;
	virtual void tick(float dt) override;
	virtual void init() override;

public:
	Bloodworks() {}
	virtual ~Bloodworks();

	MonsterController* getMonsterController()
	{
		return &monsterController;
	}

	BulletController* getBulletController()
	{
		return &bulletController;
	}

	Player* getPlayer()
	{
		return player;
	}
};