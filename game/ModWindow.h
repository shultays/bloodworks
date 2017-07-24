#pragma once

#include "UserDetails.h"
#include "cSlaveWork.h"

class Bloodworks;
class cRenderableContainer;
class cButton;
class cScrollContainer;

class ModWindow
{
	Bloodworks *bloodworks;
	cRenderableContainer *mainWindow;
	cScrollContainer *modList;
	UserDetails userDetails;

	class LoginWork : public cSlaveWork
	{
		ModWindow *modWindow;
	public:
		LoginWork(ModWindow* modWindow)
		{
			this->modWindow = modWindow;
		}
		virtual void runOnSlave()
		{
			modWindow->userDetails.tryLogin();
		}
	} loginWork;

	class FetchResults : public cSlaveWork
	{
		ModWindow *modWindow;
	public:
		FetchResults(ModWindow* modWindow)
		{
			this->modWindow = modWindow;
		}
		virtual void runOnSlave();
		virtual void runOnMain();
	} fetchResults;

	nlohmann::json modsJson;

	bool loginning;
	bool fetchingResults;
	std::vector<cButton*> modSelectButtons;
public:
	ModWindow(Bloodworks *bloodworks);
	~ModWindow();
	bool isVisible() const;
	void setVisible(bool visible);
	void tick();
};