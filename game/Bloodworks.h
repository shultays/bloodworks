#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
#include "MonsterController.h"


class cTexturedQuadRenderable;
class Player;
class Monster;
class Bloodworks : public cGame
{
	cTexturedQuadRenderable *bg;
	Player *player;


	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;

	MonsterController monsterController;
protected:
	virtual void render() override;
	virtual void tick(float dt) override;
	virtual void init() override;

public:
	virtual ~Bloodworks();
};