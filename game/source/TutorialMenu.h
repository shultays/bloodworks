#pragma once


#include "cGlobals.h"

class Bloodworks;
class cRenderableContainer;
class cTextRenderable;
class cButton;
class cTickBox;


class TutorialMenu
{
	Bloodworks *bloodworks;

	cButton *yes;
	cButton *no;

	cRenderableContainer *window;
	float alpha;
	bool joyPadFree;
	int hoverLevelupPerkIndex;
	float lastMouseMoveTimer;
public:
	TutorialMenu(Bloodworks* bloodworks);
	~TutorialMenu();
	void tick();
	bool isVisible() const;
	void show();
};