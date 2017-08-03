#pragma once

#include <string>

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"
#include "cSound.h"
#include "DirentHelper.h"
#include "BuffFloat.h"

class Bloodworks;
class cRenderable;
class Bullet;
class GameObject;
class Monster;
class GameObjectTemplate;

#include "json.h"

class MissionController
{
	Bloodworks *bloodworks;
	sol::table scriptTable;
	
	std::unordered_map<int, GameObject*> gameObjects;

	struct MissionData
	{
		std::string name;
		std::string scriptName;
		std::string scriptFile;
	};

	cVector<MissionData> missions;
	float missionLoadTime;
	int loadedMission;

	cSoundSampleWithParams missionLoop;
	cSoundHandle missionLoopHandle;
	float soundSpeed;
	float musicVolume;

	BuffFloat gameSpeedMultiplier; // this can be stored in a mission class
public:
	MissionController(Bloodworks *bloodworks);
	~MissionController();
	void tick();
	GameObject* addGameObject(const std::string& script, GameObjectTemplate *gameObjectTemplate, const sol::table& params);
	GameObject* addGameObjectUsingTemplate(const std::string& templateName, const sol::table& params);
	void removeGameObject(int id);
	void addMission(nlohmann::json& j, const DirentHelper::File& file);

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
};