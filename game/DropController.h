#pragma once

#include "cGlobals.h"

class Gun;
class Bonus;
class cRenderableGroup;
class Bloodworks;

class DropController
{
	Bloodworks *bloodworks;
	struct Drop
	{
		Gun *gun;
		Bonus *bonus;
		Vec2 pos;
		cRenderableGroup *renderable;
		float time;

		bool canFadeout;
	};

	std::vector<Drop> drops;

public:
	DropController(Bloodworks *bloodworks)
	{
		this->bloodworks = bloodworks;
	}
	~DropController();

	void createGun(const Vec2& position, int forceIndex = -1);
	void createBonus(const Vec2& position, int forceIndex = -1);
	void tick();
	void addDrop(const Vec2& position);
};