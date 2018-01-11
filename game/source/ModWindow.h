#pragma once

#include "cDefines.h"

#ifdef MOD_WINDOW

#include "UserDetails.h"
#include "cSlaveWork.h"

#include <unordered_map>

class Bloodworks;
class cRenderableContainer;
class cButton;
class cScrollContainer;
class cTextRenderable;
class cTexturedQuadRenderable;
class cTickBox;

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
	cVector<std::string> oldInstalledMods;

	void showModDetails(struct ModData& modData);

	bool detailInstalled;
	bool detailEnabled;
	cTextRenderable *detailName;
	cTexturedQuadRenderable *detailIcon;
	cTextRenderable *detailVersion;
	cTextRenderable *detailCreator;
	cTextRenderable *detailDescription;
	cTextRenderable *modLoadingText;
	cButton *detailInstallButton;
	cTextRenderable *detailInstallText;
	cTickBox *detailIsEnabled;
	cButton *detailDeleteButton;
	ModData detailedMod;

	void updateList();
	bool needsReset;
	bool closeOnNextTick;
	float closeOnNextTickSetRenderTime;

	cVector<std::string> disabledMods;
	cVector<std::string> disabledModsFullPaths;
	void updateDetailInfo();
	void syncModEnableState(const std::string& fullPath, bool detailEnabled);
	void fetchData();
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
		std::string readBuffer;
		ModWindow *modWindow;
	public:
		FetchResults(ModWindow* modWindow)
		{
			this->modWindow = modWindow;
		}
		virtual void runOnSlave();
		virtual void runOnMain();
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
		bool success;
		virtual void runOnSlave();
		virtual void runOnMain();
	} fetchMod;

	ModWindow(Bloodworks *bloodworks);
	~ModWindow();
	bool isVisible() const;
	void setVisible(bool visible, bool refresh = false);
	void tick();
	void addInstalledMod(nlohmann::json& j, DirentHelper::File& f);
	bool isPathDisabled(const std::string& path) const;
	bool isModDisabled(const std::string& mod) const;
};

#endif