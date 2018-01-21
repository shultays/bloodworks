#pragma once

#include <string>

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"
#include "cSound.h"
#include "DirentHelper.h"
#include "BuffFloat.h"
#include "json.h"
#include "cPersistent.h"
#include "MissionController.h"

class Bloodworks;
class cRenderable;
class Bullet;
class GameObject;
class Monster;
class GameObjectTemplate;

class MissionMod
{
	friend class BloodworksLuaWorld;

	sol::table scriptTable;

	sol::function onTickFunc;
	sol::function initFunc;

	cPersistent* persistent;

	sol::table data;
public:
	MissionMod(Bloodworks* bloodworks, nlohmann::json& j, const DirentHelper::File& file);
	~MissionMod();

	void init();
	void onTick();
};


struct MissionData
{
	std::string name;
	std::string icon;
	std::string description;
	std::string scriptName;
	std::string scriptFile;
	std::string basePath;

	sol::table scriptTable;

	sol::function onTickFunc;
	sol::function initFunc;

	sol::function isEnabled;
	sol::function isCompleted;

	sol::function onPlayerDied;
	sol::function onMonsterDied;

	cPersistent* persistent;

	cVector<std::string> folder;

	Vec2 mapSize;

	int guiIndex;
};


struct MissionTreeItem  
{
	bool folder;

	std::string folderPath;

	std::string name;
	std::string icon;
	std::string description;

	std::string scriptName;

	int missionIndex;
	bool enabled;
	bool completed;

	int guiSort;
};

class MissionController
{
	Bloodworks *bloodworks;
	sol::table scriptTable;
	
	std::unordered_map<int, GameObject*> gameObjects;

	cVector<MissionData*> missions;
	float missionLoadTime;
	int loadedMission;

	cSoundSampleWithParams missionLoop;
	cSoundHandle missionLoopHandle;
	float soundSpeed;
	float musicVolume;

	BuffFloat gameSpeedMultiplier; // this can be stored in a mission class

	cVector< MissionMod* > missionMods;

	cVector< sol::function > customTicks;
public:
	MissionController(Bloodworks *bloodworks);
	~MissionController();
	void tick();
	GameObject* addGameObject(const std::string& script, GameObjectTemplate *gameObjectTemplate, const sol::table& params);
	GameObject* addGameObjectUsingTemplate(const std::string& templateName, const sol::table& params);
	void removeGameObject(int id);
	void addMission(nlohmann::json& j, const DirentHelper::File& file);
	void addMissionMod(nlohmann::json& j, const DirentHelper::File& file);

	void loadMission(const std::string& name);
	void reset();
	void onPlayerDied();
	bool isLoaded() const;

	sol::table getMissionData();
	void onMonsterDied(Monster* monster);
	void repositionGUI();
	void updateMusicVolume();
	void onDebugTick();
	float getGameSpeedMultiplier()
	{
		return gameSpeedMultiplier.getBuffedValue();
	}
	BuffFloat& getGameSpeedMultiplierBuff()
	{
		return gameSpeedMultiplier;
	}

	float getMissionStartTime() const
	{
		return missionLoadTime;
	}

	std::string getCurrentMissionScript();

	std::unordered_map<std::string, cPersistent*> persistents;

	cPersistent* getPersistent(const std::string& path);
	void getMissions(cVector<MissionTreeItem>& currentMods, const cVector<std::string>& folder) const;
	void addMissionFolderInfo(const nlohmann::json& j, DirentHelper::File& f);

	struct FolderInfo
	{
		std::string name;
		std::string icon;
		std::string description;
	};
	std::unordered_map<std::string, FolderInfo> folderInfos;

	void addCustomTick(sol::function& func)
	{
		customTicks.push_back(func);
	}
};