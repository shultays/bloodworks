#pragma once

#include <vector>

class Bloodworks;
class cRenderableGroup;
class cTextRenderable;
class cButton;
class cTickBox;
class cSlider;
class cKeyMapButton;

class OptionsPopup
{
	Bloodworks *bloodworks;

	cRenderableGroup *optionsGroup;

	// gameplay options
	cButton *gameplayTitle;
	cRenderableGroup *gameplayGroup;

	cTickBox *gore;
	cTickBox *screenShake;
	cTickBox *lockCrosshair;
	cTickBox *autoLevelUp;

	// audio video options
	cButton *audioVideoTitle;
	cRenderableGroup *audioVideoGroup;

	cTickBox *fullScreen;
	cTickBox *vsync;

	cSlider *volume;
	cSlider *musicVolume;


	// input settings
	cButton *inputTitle;
	cRenderableGroup *inputGroup;

	cSlider *sensitivity;

	cKeyMapButton *inUseKey;
	std::vector<cKeyMapButton*> keyMapButtons;

	//
	cButton *lastClickedTitle;
	cButton *prevClickedTitle;
	float lastClickTime;

	cRenderableGroup *lastClickedGroup;
	cRenderableGroup *prevClickedGroup;

	bool changingTabs;

	void changeTab(cButton * tab, cRenderableGroup *group);
public:
	OptionsPopup(Bloodworks *bloodworks);
	~OptionsPopup();
	bool isVisible() const;
	void show();
	void tick();
};
