#pragma once

#include <vector>

class Bloodworks;
class cRenderableContainer;
class cTextRenderable;
class cButton;
class cTickBox;
class cSlider;
class cKeyMapButton;
class cScrollContainer;
class cRenderable;

class OptionsPopup
{
	Bloodworks *bloodworks;

	cRenderableContainer *optionsGroup;

	// gameplay options
	cButton *gameplayTitle;
	cRenderableContainer *gameplayGroup;

	cTickBox *gore;
	cTickBox *screenShake;
	cTickBox *lockCrosshair;
	cTickBox *autoLevelUp;

	// audio video options
	cButton *audioVideoTitle;
	cRenderableContainer *audioVideoGroup;

	cTickBox *fullScreen;
	cTickBox *vsync;

	cSlider *volume;
	cSlider *musicVolume;


	// input settings
	cButton *inputTitle;
	cScrollContainer *inputGroup;

	cSlider *sensitivity;

	cKeyMapButton *inUseKey;
	std::vector<cKeyMapButton*> keyMapButtons;

	//
	cButton *lastClickedTitle;
	cButton *prevClickedTitle;
	float lastClickTime;

	cRenderable *lastClickedGroup;
	cRenderable *prevClickedGroup;

	bool changingTabs;

	void changeTab(cButton *tab, cRenderable *group);
public:
	OptionsPopup(Bloodworks *bloodworks);
	~OptionsPopup();
	bool isVisible() const;
	void show();
	void tick();
};
