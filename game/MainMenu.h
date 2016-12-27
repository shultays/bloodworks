#pragma once

class Bloodworks;
class cTexturedQuadRenderable;
class cButton;

class MainMenu
{
	Bloodworks *bloodworks;
	cTexturedQuadRenderable *bloodworksText;

	cButton *newGame;
	cButton *mods;
	cButton *options;
	cButton *credits;
	cButton *quit;
public:
	MainMenu(Bloodworks *b);
	~MainMenu();
	void resize();
	void tick();
	void setVisible(bool visible);
};