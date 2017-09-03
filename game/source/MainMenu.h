#pragma once

#include "cResources.h"
#include "cSound.h"

class Bloodworks;
class cTexturedQuadRenderable;
class cButton;

class MainMenu
{
	Bloodworks *bloodworks;
	cTexturedQuadRenderable *bloodworksText;

	cButton *newGame;
	cButton *customGames;
	cButton *mods;
	cButton *options;
	cButton *credits;
	cButton *quit;

	cSoundSampleShr music;
	cSoundHandle handle;

public:
	MainMenu(Bloodworks *b);
	~MainMenu();
	void resize();
	void tick(bool hasPopup);
	void setVisible(bool visible);
	void updateMusicVolume();
};