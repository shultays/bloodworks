#pragma once

#include "cGlobals.h"

class Bloodworks;
class cRenderableGroup;
class cTextRenderable;
class cRenderable;
class Perk;


class LevelUpPopup
{
	Bloodworks *bloodworks;

	cRenderableGroup *levelupGroup;

	cTextRenderable *levelupGroupTitle;
	cTextRenderable *currentPerkName;
	cTextRenderable *currentPerkExplanation;
	std::vector<cRenderable*> levelupPerksRenderables;
	std::vector<Vec2> levelupPerksRenderablePosition;
	std::vector<Perk*> levelupPerks;
	int hoverLevelupPerkIndex;

public:

	LevelUpPopup(Bloodworks *bloodworks);
	~LevelUpPopup();
	bool isVisible() const;
	void show();
	void tick();
};