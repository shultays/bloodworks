#pragma once


#include "cGlobals.h"

class Bloodworks;
class cRenderableContainer;
class cTextRenderable;
class cTexturedQuadRenderable;


class CreditsWindow
{
	Bloodworks *bloodworks;
	cRenderableContainer *window;
	cTextRenderable *credits;
	cTexturedQuadRenderable *bg;
	float textY;
	float visibleTime;
public:
	CreditsWindow(Bloodworks* bloodworks);
	~CreditsWindow();
	void tick();
	void setVisible(bool visible);
	bool isVisible() const;
private:
	void resetTextPos();
};