#pragma once

#include "cGlobals.h"

class Bloodworks;
class cRenderableGroup;
class cTextRenderable;
class cButton;
class Perk;


class LevelUpPopup
{
	Bloodworks *bloodworks;

	cRenderableGroup *levelupGroup;

	cTextRenderable *levelupGroupTitle;
	cTextRenderable *currentPerkName;
	cTextRenderable *currentPerkExplanation;
	std::vector<cButton*> levelupPerksRenderables;
	std::vector<Perk*> levelupPerks;
	int hoverLevelupPerkIndex;
	cSoundSampleShr levelUpSound;
public:

	LevelUpPopup(Bloodworks *bloodworks);
	~LevelUpPopup();
	bool isVisible() const;
	void show();
	void tick();
};