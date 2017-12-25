#pragma once

#include "steam_api.h" 
#include "cVector.h"
#include <unordered_map>

enum EAchievement { 
	ACH_TEST = 0,
	ACH_BRONZE,
	ACH_SILVER,
	ACH_GOLD,
	ACH_PISTOL,
	ACH_RIFLE,
	ACH_SPLIT_GUN,
	ACH_RAIL_GUN,
	ACH_SHRINK_GUN,
	ACH_FLAME_THROWER,
	ACH_FREEZE_GUN,
	ACH_PLASMA_GUN,
	ACH_BLACK_HOLE,
	ACH_PHOENIX_DIVE,
	ACH_NAR_SIE,
	ACH_SHOTGUN,

	ACH_COUNT
};
enum EStat { STA_TEST = 0 };

class Bloodworks;

class BloodworksSteam
{
private:
	bool inited;
	Bloodworks* bloodworks;

	std::unordered_map<std::string, EAchievement > nameMap;
public:
	BloodworksSteam(Bloodworks* bloodworks);

	void addAchievement(EAchievement achivement);
	bool hasAchievement(EAchievement achivement) const;
	void addStat(EStat stat, int value);

	void addAchievement(const std::string& achivement);
	bool hasAchievement(const std::string& achivement) const;

	void resetUser();

	void tick();
	void reset();
};