#pragma once

#include "steam_api.h" 

enum EAchievements { ACH_TEST = 0 };
enum EStats { STA_TEST = 0 };


class BloodworksSteam
{
private:
	bool inited;
public:
	BloodworksSteam();

	void addAchievement(EAchievements achivement);
	void addStat(EStats stat, int value);

	void resetUser();
};