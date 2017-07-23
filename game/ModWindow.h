#pragma once

#include "UserDetails.h"

class Bloodworks;
class cRenderableContainer;
class ModWindow
{
	Bloodworks *bloodworks;
	cRenderableContainer *mainWindow;
	UserDetails userDetails;

public:
	ModWindow(Bloodworks *bloodworks);
	~ModWindow();


	void tick();
};