#pragma once

#include "UserDetails.h"
#include "cSlaveWork.h"
#include <unordered_map>

class Bloodworks;
class cRenderableContainer;
class cButton;
class cScrollContainer;

class ModWindow
{
	Bloodworks *bloodworks;
	cRenderableContainer *mainWindow;
	cRenderableContainer *modListWindow;
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


	bool loginning;
	bool fetchingResults;
	std::vector<cButton*> modSelectButtons;

	struct ModData
	{
		nlohmann::json jsonData;
		DirentHelper::File filePath;
	};
	std::vector<struct ModData> loadedMods;
	std::vector<struct ModData> installedMods;
	std::unordered_map<std::string, int> installedModIndices;
public:
	ModWindow(Bloodworks *bloodworks);
	~ModWindow();
	bool isVisible() const;
	void setVisible(bool visible);
	void tick();
	void addInstalledMod(nlohmann::json& j, DirentHelper::File& f);
};