#pragma once

#include <string>

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"
#include "cSound.h"

class Bloodworks;
class cRenderable;
class Bullet;
class GameObject;

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

	std::vector<MissionData> missions;
	float missionLoadTime;
	int loadedMission;

	cSoundSampleWithParams missionLoop;
	cSoundHandle missionLoopHandle;
	float soundSpeed;
	float musicVolume;
public:
	MissionController(Bloodworks *bloodworks);
	~MissionController();
	void tick();
	GameObject* addGameObject(const std::string& script);
	void removeGameObject(int id);
	void addMission(nlohmann::json &j);

	void loadMission(const std::string& name);
	void reset();
	void onPlayerDied();
	bool isLoaded() const;
};