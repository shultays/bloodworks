#pragma once

#include "BloodworksSteam.h"
#include "cSteam.h"
#include "cGlobals.h"


#define _ACH_ID( id ) { id, #id, false } 
Achievement_t g_Achievements[] = { _ACH_ID(ACH_TEST), };
#define _STAT_ID( id,type ) { id, #id, type, 0, 0, 0, 0 }
Stat_t g_Stats[] = { _STAT_ID(STA_TEST, STAT_INT), };


BloodworksSteam::BloodworksSteam()
{
	inited = coral.getSteam() && coral.getSteam()->isInited();

	if (inited)
	{
		coral.getSteam()->init(g_Achievements, sizeof(g_Achievements) / sizeof(g_Achievements[0]),
			g_Stats, sizeof(g_Stats) / sizeof(g_Stats[0]));
	}
}

void BloodworksSteam::addAchievement(EAchievements achivement)
{
	if (inited)
	{
		coral.getSteam()->getAchievements()->setAchievement(g_Achievements[achivement].m_pchAchievementID);
	}
}

void BloodworksSteam::addStat(EStats stat, int value)
{
	if (inited)
	{
		coral.getSteam()->getAchievements()->setStat(g_Stats[stat].m_pchID, g_Stats[stat].m_iValue + value);
	}
}

void BloodworksSteam::resetUser()
{
	if (inited)
	{
		coral.getSteam()->getAchievements()->resetUser();
	}
}
