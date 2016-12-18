#pragma once

#include "cRenderable.h"
#include "json.h"

class Bloodworks;

class LaserRenderable : public cRenderableGroup
{
	cTexturedQuadRenderable *laserBeginRenderable;
	cTexturedQuadRenderable *laserMidrenderable;
	cTexturedQuadRenderable *laserEndRenderable;
	float laserLength;
	float laserScale;
	Vec2 laserBeginShift;
	Vec2 laserEndShift;

public:
	LaserRenderable(Bloodworks *bloodworks);
private:
	void updateRenderables();
};