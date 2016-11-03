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
protected:
	virtual void tick(float dt);

	virtual void init();

};