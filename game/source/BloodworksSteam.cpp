#pragma once

#include "BloodworksSteam.h"
#include "cSteam.h"
#include "cGlobals.h"
#include "Bloodworks.h"
#include "MissionController.h"
#include "Player.h"
#include "Gun.h"
#include "MonsterController.h"
#include "Monster.h"


#define _ACH_ID( id ) { id, #id, false } 
Achievement_t g_Achievements[] = {
	_ACH_ID(ACH_TEST),
	_ACH_ID(ACH_BRONZE),
	_ACH_ID(ACH_SILVER),
	_ACH_ID(ACH_GOLD),
	_ACH_ID(ACH_PISTOL),
	_ACH_ID(ACH_RIFLE),
	_ACH_ID(ACH_SPLIT_GUN),
	_ACH_ID(ACH_RAIL_GUN),
	_ACH_ID(ACH_SHRINK_GUN),
	_ACH_ID(ACH_FLAME_THROWER),
	_ACH_ID(ACH_FREEZE_GUN),
	_ACH_ID(ACH_PLASMA_GUN),
	_ACH_ID(ACH_BLACK_HOLE),
	_ACH_ID(ACH_PHOENIX_DIVE),
	_ACH_ID(ACH_NAR_SIE),
	_ACH_ID(ACH_SHOTGUN),
};
#define _STAT_ID( id,type ) { id, #id, type, 0, 0, 0, 0 }
Stat_t g_Stats[] = { _STAT_ID(STA_TEST, STAT_INT), };

BloodworksSteam::BloodworksSteam(Bloodworks* bloodworks)
{
	this->bloodworks = bloodworks;
	inited = coral.getSteam() && coral.getSteam()->isInited();

	if (inited)
	{
		coral.getSteam()->init(g_Achievements, sizeof(g_Achievements) / sizeof(g_Achievements[0]),
			g_Stats, sizeof(g_Stats) / sizeof(g_Stats[0]));
	}

	for (int n = 0; n < sizeof(g_Achievements) / sizeof(g_Achievements[0]); n++)
	{
		nameMap[g_Achievements[n].m_pchAchievementID] = (EAchievement) n;
	}
	reset();
}

void BloodworksSteam::addAchievement(EAchievement achivement)
{
	if (inited)
	{
		coral.getSteam()->getAchievements()->setAchievement(g_Achievements[achivement].m_pchAchievementID);
		g_Achievements[achivement].m_bAchieved = true;
	}
}

void BloodworksSteam::addAchievement(const std::string& achivement)
{
	if (inited)
	{
		auto t = nameMap.find(achivement);
		if (t != nameMap.end())
		{
			addAchievement((*t).second);
		}
		else
		{
			out << "No achievement with name: " << achivement << "\n";
		}
	}
}

bool BloodworksSteam::hasAchievement(EAchievement achivement) const
{
	if (inited)
	{
		return g_Achievements[achivement].m_bAchieved;
	}
	return false;
}

bool BloodworksSteam::hasAchievement(const std::string& achivement) const
{
	if (inited)
	{
		auto t = nameMap.find(achivement);
		if (t != nameMap.end())
		{
			return hasAchievement((*t).second);
		}
		else
		{
			out << "No achievement with name: " << achivement << "\n";
			return false;
		}
	}
	return false;
}

void BloodworksSteam::addStat(EStat stat, int value)
{
	if (inited)
	{
		coral.getSteam()->getAchievements()->setStat(g_Stats[stat].m_pchID, g_Stats[stat].m_iValue += value);
	}
}

void BloodworksSteam::resetUser()
{
	if (inited)
	{
		coral.getSteam()->getAchievements()->resetUser();
	}
}

void BloodworksSteam::tick()
{
	if (!inited || !bloodworks->getMissionController()->isLoaded() || !bloodworks->getPlayer()->isVisible())
	{
		return;
	}

	float time = timer.getTime();
	if (time > 2 * 60.0f && !g_Achievements[ACH_BRONZE].m_bAchieved)
	{
		addAchievement(ACH_BRONZE);
	}
	if (time > 5 * 60.0f && !g_Achievements[ACH_SILVER].m_bAchieved)
	{
		addAchievement(ACH_SILVER);
	}
	if (time > 8 * 60.0f && !g_Achievements[ACH_GOLD].m_bAchieved)
	{
		addAchievement(ACH_GOLD);
	}
}

void BloodworksSteam::reset()
{
}
