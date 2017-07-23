#pragma once

#include "UserDetails.h"
#include "cSlaveWork.h"

class Bloodworks;
class cRenderableContainer;
class ModWindow
{
	Bloodworks *bloodworks;
	cRenderableContainer *mainWindow;
	UserDetails userDetails;

	class cLoginWork : public cSlaveWork
	{
		ModWindow *modWindow;
	public:
		cLoginWork(ModWindow* modWindow)
		{
			this->modWindow = modWindow;
		}
		virtual void runOnSlave()
		{
			modWindow->userDetails.tryLogin();
		}
	} loginWork;

public:
	ModWindow(Bloodworks *bloodworks);
	~ModWindow();

	void tick();
};