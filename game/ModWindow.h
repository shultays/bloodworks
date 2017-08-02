#pragma once

#include "UserDetails.h"
#include "cSlaveWork.h"
#include <unordered_map>

class Bloodworks;
class cRenderableContainer;
class cButton;
class cScrollContainer;
class cTextRenderable;
class cTexturedQuadRenderable;

class ModWindow
{
	Bloodworks *bloodworks;
	cRenderableContainer *mainWindow;
	cRenderableContainer *modListWindow;
	cRenderableContainer *modDetailWindow;
	cScrollContainer *modList;
	UserDetails userDetails;

	int fetchingImageId;

	bool loginning;
	bool fetchingResults;
	cVector<cButton*> modSelectButtons;

	struct ModData
	{
		nlohmann::json jsonData;
		DirentHelper::File filePath;
	};
	cVector<struct ModData> loadedMods;
	cVector<struct ModData> installedMods;
	std::unordered_map<std::string, int> installedModIndices;

	void showModDetails(struct ModData& modData);

	bool detailInstalled;
	cTextRenderable *detailName;
	cTexturedQuadRenderable *detailIcon;
	cTextRenderable *detailVersion;
	cTextRenderable *detailCreator;
	cTextRenderable *detailDescription;
	cButton *detailInstallButton;
	cTextRenderable *detailInstallText;
	ModData detailedMod;

	void updateList();

public:

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
	} fetchResults;

	class FetchImage : public cSlaveWork
	{
	public:
		ModWindow *modWindow;
		int id;
		virtual void runOnSlave();
		virtual void runOnMain();
	} fetchImage;

	class FetchMod : public cSlaveWork
	{
	public:
		std::string readBuffer;
		int byteToLoad;
		ModWindow *modWindow;
		int id;
		bool valid;
		std::string installPath;
		virtual void runOnSlave();
		virtual void runOnMain();
	} fetchMod;

	ModWindow(Bloodworks *bloodworks);
	~ModWindow();
	bool isVisible() const;
	void setVisible(bool visible);
	void tick();
	void addInstalledMod(nlohmann::json& j, DirentHelper::File& f);
};