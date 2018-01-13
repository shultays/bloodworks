#pragma once

#include "cResources.h"
#include "cSound.h"

class Bloodworks;
class cTexturedQuadRenderable;
class cButton;
class cTextRenderable;

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


	cTextRenderable* topScore;
public:
	MainMenu(Bloodworks *b);
	~MainMenu();
	void resize();
	void tick(bool hasPopup);
	void setVisible(bool visible);
	void updateMusicVolume();

	void updateScore(const std::string& score);
	void invalidateScore();
};