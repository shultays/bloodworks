#pragma once

#include "cGlobals.h"

class Bloodworks;
class cRenderableContainer;
class cTextRenderable;
class cButton;
class Perk;


class LevelUpPopup
{
	Bloodworks *bloodworks;

	cRenderableContainer *levelupGroup;

	cTextRenderable *levelupGroupTitle;
	cTextRenderable *levelupGroupSubTitle;
	cTextRenderable *currentPerkName;
	cTextRenderable *currentPerkExplanation;
	std::vector<cButton*> levelupPerksRenderables;
	std::vector<Perk*> levelupPerks;
	int hoverLevelupPerkIndex;
	cSoundSampleShr levelUpSound;

	float lastMouseMoveTimer;
	bool joyPadFree;
	int waitingLevels;

	cTextRenderable *levelUpText;
	float levelUpShowTime;
	void clearPerks();
public:
	LevelUpPopup(Bloodworks *bloodworks);
	~LevelUpPopup();
	bool isVisible() const;
	void show(bool levelAdded = true);
	void tick();

	int getWaitingLevels() const
	{
		return waitingLevels;
	}

	void reset();
	void showLevelUpText();
};