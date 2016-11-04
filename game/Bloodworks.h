#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
class cTexturedQuadRenderable;
class Player;
class Monster;
class Bloodworks : public cGame
{
	cTexturedQuadRenderable *bg;
	Player *player;

	std::vector<Monster*> monsters;

	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;

protected:
	virtual void render() override;
	virtual void tick(float dt) override;
	virtual void init() override;

public:
	virtual ~Bloodworks();
};