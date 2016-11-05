#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
#include "MonsterController.h"


class cTexturedQuadRenderable;
class Player;
class Monster;
class Bullet;

class Bloodworks : public cGame
{
	cTexturedQuadRenderable *bg;
	Player *player;


	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;


	std::vector<Bullet*> bullets;
protected:
	virtual void render() override;
	virtual void tick(float dt) override;
	virtual void init() override;

public:
	MonsterController monsterController;
	virtual ~Bloodworks();
	void addBullet(Bullet *bullet);
};