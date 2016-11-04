#pragma once

class Bloodworks;
class cAnimatedTexturedQuadRenderable;

class Monster
{
	cAnimatedTexturedQuadRenderable *renderable;
	Bloodworks *bloodworks;

public:
	Monster(Bloodworks *bloodworks);
	~Monster();
	void tick(float dt);
};