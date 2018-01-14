#pragma once

#include "cGlobals.h"
#include "MissionController.h"

class Bloodworks;
class cRenderableContainer;
class cTextRenderable;
class cScrollContainer;
class cButton;

class CustomGameWindow
{
	Bloodworks *bloodworks;

	cRenderableContainer *window;
	cTextRenderable *title;
	cTextRenderable *nomods;
	cTextRenderable *currentDescription;

	cScrollContainer *buttonContainer;
	cVector<cButton*> customGameButtons;

	void clearButtons();
	void loadMods();
public:
	CustomGameWindow(Bloodworks *bloodworks);
	~CustomGameWindow();
	bool isVisible() const;
	void show();
	void tick();

	cVector<std::string> folder;

	cVector<MissionTreeItem> currentMods;
};