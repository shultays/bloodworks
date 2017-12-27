#pragma once


#include "cGlobals.h"

class Bloodworks;
class cRenderableContainer;
class cTextRenderable;
class cButton;
class cTickBox;


class PauseMenu
{
	Bloodworks *bloodworks;

	cButton *restart;
	cButton *options;
	cButton *quit;

	cRenderableContainer *window;
	float alpha;


	float lastMouseMoveTimer;
public:
	PauseMenu(Bloodworks* bloodworks);
	~PauseMenu();
	void tick();
private:
	bool joyPadFree;
	int hoverLevelupPerkIndex;
};