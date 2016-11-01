#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
class cTexturedQuadRenderable;
class Bloodworks : public cGame
{
	cShaderShr s;
	cTexturedQuadRenderable *bg;
	cTexturedQuadRenderable *body;
	cTexturedQuadRenderable *hands;

	float angle;
	float speed;
	Vec2 pos;
protected:
	virtual void tick(float dt);

	virtual void init();

};