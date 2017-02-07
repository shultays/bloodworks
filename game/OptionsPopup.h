#pragma once

class Bloodworks;
class cRenderableGroup;
class cTextRenderable;
class cButton;

class OptionsPopup
{
	Bloodworks *bloodworks;

	cRenderableGroup *optionsGroup;

	// gameplay options
	cButton *gameplayTitle;
	cRenderableGroup *gameplayGroup;

	// audio video options
	cButton *audioVideoTitle;
	cRenderableGroup *audioVideoGroup;

	// input settings
	cButton *inputTitle;
	cRenderableGroup *inputGroup;

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
