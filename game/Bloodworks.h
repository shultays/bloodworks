#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
class cTexturedQuadRenderable;
class Player;
class Monster;
class Bloodworks : public cGame
{
	cTexturedQuadRenderable *bg;
	Monster *monster;
	Player *player;
protected:
	virtual void tick(float dt);

	virtual void init();

};