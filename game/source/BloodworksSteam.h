#pragma once

#include "cVector.h"
#include "cDefines.h"

#include <unordered_map>


#define MAX_WORKSHOP_ITEMS 512

enum EAchievement 
{ 
	ACH_BRONZE,
	ACH_SILVER,
	ACH_GOLD,
	ACH_PISTOL,
	ACH_RIFLE,
	ACH_SPLIT_GUN,
	ACH_RAIL_GUN,
	ACH_SHRINK_GUN,
	ACH_FLAME_THROWER,
	ACH_FROST_LASER,
	ACH_PLASMA_GUN,
	ACH_BLACK_HOLE,
	ACH_PHOENIX_DIVE,
	ACH_NAR_SIE,
	ACH_SHOTGUN,
	ACH_BLOODBATH,
	ACH_LIVING_ON_THE_EDGE,
	ACH_CARNAGE,
	ACH_MERCILESS,
	ACH_ROCKET_LAUNCHER,
	ACH_MINIGUN,
	ACH_MOD,
	
	ACH_COUNT
};

enum EStat 
{
	STA_BLOODBATH,

	STA_COUNT
};


enum ELeaderBoard
{
	LED_SCORE,
	LED_LIFE_TIME,

	LED_COUNT
};

#ifdef HAS_STEAM

#include "steam_api.h"

class Bloodworks;
class Monster;

class BloodworksSteam
{
private:
	bool inited;
	Bloodworks* bloodworks;

	std::unordered_map<std::string, EAchievement > nameMap;
	int killedMonster;
	float edgeTime;

	int highScore;
	int totalTime;

	bool updatingScores;
public:
	BloodworksSteam(Bloodworks* bloodworks);

	void addAchievement(EAchievement achivement);
	bool hasAchievement(EAchievement achivement) const;
	void addStat(EStat stat, int value);

	void addAchievement(const std::string& achivement);
	bool hasAchievement(const std::string& achivement) const;

	void resetUser();

	void init();
	void tick();
	void reset();
	void onMonsterDied(Monster* monster);


	int m_nNumWorkshopItems = 0;
	bool loadWorkshopItem(PublishedFileId_t workshopItemID);
	void loadWorkshopItems();
	STEAM_CALLBACK(BloodworksSteam, onWorkshopItemInstalled, ItemInstalled_t);

	void openWorkshop();

	void uploadLeaderboards(int score, float time);
	void updateMainMenuScore();
};

#else

class Bloodworks;
class Monster;

class BloodworksSteam
{
private:
public:
	BloodworksSteam(Bloodworks* bloodworks) {}

	void addAchievement(EAchievement achivement) {}
	bool hasAchievement(EAchievement achivement) const { return true;  }
	void addStat(EStat stat, int value) {}

	void addAchievement(const std::string& achivement) {}
	bool hasAchievement(const std::string& achivement) const { return true; }

	void resetUser() {}

	void init() {}
	void tick() {}
	void reset() {}
	void onMonsterDied(Monster* monster) {}
	void updateMainMenuScore();
};

#endif

