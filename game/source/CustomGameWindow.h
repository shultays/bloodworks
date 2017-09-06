#pragma once

#include "cGlobals.h"

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
	cVector<std::string> customGameScripts;
	cVector<std::string> customGameNames;
	cVector<std::string> customGameDescriptions;

	void clearButtons();
	void loadMods();

public:
	CustomGameWindow(Bloodworks *bloodworks);
	~CustomGameWindow();
	bool isVisible() const;
	void show();
	void tick();
};