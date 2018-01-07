#pragma once

#include "BloodworksSteam.h"

#ifdef HAS_STEAM
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
	_ACH_ID(ACH_BRONZE),
	_ACH_ID(ACH_SILVER),
	_ACH_ID(ACH_GOLD),
	_ACH_ID(ACH_PISTOL),
	_ACH_ID(ACH_RIFLE),
	_ACH_ID(ACH_SPLIT_GUN),
	_ACH_ID(ACH_RAIL_GUN),
	_ACH_ID(ACH_SHRINK_GUN),
	_ACH_ID(ACH_FLAME_THROWER),
	_ACH_ID(ACH_FROST_LASER),
	_ACH_ID(ACH_PLASMA_GUN),
	_ACH_ID(ACH_BLACK_HOLE),
	_ACH_ID(ACH_PHOENIX_DIVE),
	_ACH_ID(ACH_NAR_SIE),
	_ACH_ID(ACH_SHOTGUN),
	_ACH_ID(ACH_BLOODBATH),
	_ACH_ID(ACH_LIVING_ON_THE_EDGE),
	_ACH_ID(ACH_CARNAGE),
	_ACH_ID(ACH_MERCILESS),
	_ACH_ID(ACH_ROCKET_LAUNCHER),
	_ACH_ID(ACH_MINIGUN),
	_ACH_ID(ACH_MOD),
};

#define _STAT_ID( id,type ) { id, #id, type, 0, 0, 0, 0 }
Stat_t g_Stats[] = {
	_STAT_ID(STA_BLOODBATH, STAT_INT),
};

void BloodworksSteam::openWorkshop()
{
	if (inited)
	{
		SteamFriends()->ActivateGameOverlayToWebPage("http://steamcommunity.com/workshop/browse/?appid=717990&browsesort=trend&section=readytouseitems");
	}
}

BloodworksSteam::BloodworksSteam(Bloodworks* bloodworks)
{
	this->bloodworks = bloodworks;
	inited = false;
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
	return true;
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

void BloodworksSteam::init()
{
	if (inited)
	{
		return;
	}
	out << "BloodworksSteam::init\n";
	inited = coral.getSteam() && coral.getSteam()->isInited();

	if (inited)
	{
		coral.getSteam()->init(g_Achievements, sizeof(g_Achievements) / sizeof(g_Achievements[0]),
			g_Stats, sizeof(g_Stats) / sizeof(g_Stats[0]));
	}

	for (int n = 0; n < sizeof(g_Achievements) / sizeof(g_Achievements[0]); n++)
	{
		nameMap[g_Achievements[n].m_pchAchievementID] = (EAchievement)n;
	}
	assert(sizeof(g_Achievements) / sizeof(g_Achievements[0]) == ACH_COUNT);
	assert(sizeof(g_Stats) / sizeof(g_Stats[0]) == STA_COUNT);
	reset();
	out << "BloodworksSteam::init fin\n";
}

void BloodworksSteam::tick()
{
	if (bloodworks->isFirstTick())
	{
		out << "BloodworksSteam::tick\n";
	}
	if (!inited || !bloodworks->getMissionController()->isLoaded() || !bloodworks->getPlayer()->isVisible())
	{
		return;
	}

	if (!(bool)bloodworks->getMissionController()->getMissionData()["isSurvival"])
	{
		return;
	}

	float time = timer.getTime() - bloodworks->getMissionController()->getMissionStartTime();
	if (time > 2 * 60.0f && !g_Achievements[ACH_BRONZE].m_bAchieved)
	{
		addAchievement(ACH_BRONZE);
	}
	if (time > 4 * 60.0f && !g_Achievements[ACH_SILVER].m_bAchieved)
	{
		addAchievement(ACH_SILVER);
	}
	if (time > 6 * 60.0f && !g_Achievements[ACH_GOLD].m_bAchieved)
	{
		addAchievement(ACH_GOLD);
	}

	if (killedMonster > 0 && !g_Achievements[ACH_BLOODBATH].m_bAchieved)
	{
		addStat(STA_BLOODBATH, killedMonster);
		killedMonster = 0;
	}

	if (!g_Achievements[ACH_LIVING_ON_THE_EDGE].m_bAchieved)
	{
		int count = 0;
		std::function<bool(Monster *monster)> func = [&](Monster *monster) -> bool
		{
			count++;
			return count < 5;
		};
		bloodworks->getMonsterController()->runForEachMonsterInRadius(bloodworks->getPlayer()->getPosition(), 140.0f, func);

		if (count >= 4)
		{
			edgeTime += timer.getDt();
			if (edgeTime >= 20.0f)
			{
				addAchievement(ACH_LIVING_ON_THE_EDGE);
			}
		}
		else
		{
			edgeTime -= timer.getDt() * 5.0f;
			edgeTime = max(edgeTime, 0.0f);
		}

	}
}

void BloodworksSteam::reset()
{
	killedMonster = 0;
	edgeTime = 0.0f;
}

void BloodworksSteam::onMonsterDied(Monster* monster)
{
	killedMonster++;
}

bool BloodworksSteam::loadWorkshopItem(PublishedFileId_t workshopItemID)
{
	if (m_nNumWorkshopItems == MAX_WORKSHOP_ITEMS)
		return false; // too much

	uint32 unItemState = SteamUGC()->GetItemState(workshopItemID);

	if (!(unItemState & k_EItemStateInstalled))
		return false;

	uint32 unTimeStamp = 0;
	uint64 unSizeOnDisk = 0;
	char szItemFolder[1024] = { 0 };

	if (!SteamUGC()->GetItemInstallInfo(workshopItemID, &unSizeOnDisk, szItemFolder, sizeof(szItemFolder), &unTimeStamp))
		return false;

	// char szFile[1024];
	// if (unItemState & k_EItemStateLegacyItem)
	// {
	// 	// szItemFolder just points directly to the item for legacy items that were published with the RemoteStorage API.
	// 	_snprintf(szFile, sizeof(szFile), "%s", szItemFolder);
	// }
	// else
	// {
	// 	_snprintf(szFile, sizeof(szFile), "%s/workshopitem.txt", szItemFolder);
	// }

	std::string folder = szItemFolder;
	out << "loading mod : " << folder << "\n";
	bloodworks->loadMod(szItemFolder);

	if (!hasAchievement(ACH_MOD))
	{
		addAchievement(ACH_MOD);
	}

	return true;
}

void BloodworksSteam::loadWorkshopItems()
{
	if (inited)
	{
		PublishedFileId_t vecSubscribedItems[MAX_WORKSHOP_ITEMS];

		int numSubscribedItems = SteamUGC()->GetSubscribedItems(vecSubscribedItems, MAX_WORKSHOP_ITEMS);

		if (numSubscribedItems > MAX_WORKSHOP_ITEMS)
		{
			numSubscribedItems = MAX_WORKSHOP_ITEMS;
		}

		for (int iSubscribedItem = 0; iSubscribedItem < numSubscribedItems; iSubscribedItem++)
		{
			PublishedFileId_t workshopItemID = vecSubscribedItems[iSubscribedItem];
			loadWorkshopItem(workshopItemID);
		}
	}
}

void BloodworksSteam::onWorkshopItemInstalled(ItemInstalled_t *pParam)
{
	if (pParam->m_unAppID == SteamUtils()->GetAppID())
	{
		if (bloodworks->isMissionLoaded())
		{
			bloodworks->gotoMainMenu();
		}
		loadWorkshopItem(pParam->m_nPublishedFileId);
	}
}
#endif