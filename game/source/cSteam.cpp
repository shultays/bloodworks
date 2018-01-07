#pragma once

#include "cSteam.h"
#include "cGlobals.h"


#ifdef HAS_STEAM

CSteamAchievements::CSteamAchievements()
	: m_iAppID(0)
	, m_bInitialized(false)
	, m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived)
	, m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored)
	, m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
{
	inited = false;
}

void CSteamAchievements::init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats)
{
	inited = true;
	m_iAppID = SteamUtils()->GetAppID();
	m_pAchievements = Achievements;
	m_iNumAchievements = NumAchievements;

	m_pStats = Stats;
	m_iNumStats = NumStats;
	dirtyStats = false;
	requestStats();
}

CSteamAchievements::~CSteamAchievements()
{
}

bool CSteamAchievements::requestStats()
{
	// Is Steam loaded? If not we can't get stats. 
	if (NULL == SteamUserStats() || NULL == SteamUser())
	{
		return false;
	}
	// Is the user logged on?  If not we can't get stats. 
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
	// Request user stats. 
	return SteamUserStats()->RequestCurrentStats();
}

bool CSteamAchievements::setAchievement(const char* ID) {
	// Have we received a call back from Steam yet? 
	if (m_bInitialized)
	{

		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	// If not then we can't set achievements yet 
	return false;
}

bool CSteamAchievements::storeStats()
{
	if (m_bInitialized)
	{
		// load stats 
		for (int iStat = 0; iStat < m_iNumStats; ++iStat)
		{
			Stat_t &stat = m_pStats[iStat];
			switch (stat.m_eStatType)
			{
			case STAT_INT:
				SteamUserStats()->SetStat(stat.m_pchID, stat.m_iValue);
				break;
			case STAT_FLOAT:
				SteamUserStats()->SetStat(stat.m_pchID, stat.m_flValue);
				break;
			case STAT_AVGRATE:
				SteamUserStats()->UpdateAvgRateStat(stat.m_pchID, stat.m_flAvgNumerator, stat.m_flAvgDenominator);
				// The averaged result is calculated for us 
				SteamUserStats()->GetStat(stat.m_pchID, &stat.m_flValue);
				break;
			default:
				break;
			}
		}
		return SteamUserStats()->StoreStats();
	}
	return false;
}

bool CSteamAchievements::setStat(const char* ID, int val)
{
	if (m_bInitialized)
	{
		SteamUserStats()->SetStat(ID, val);
		return SteamUserStats()->StoreStats();
	}
	return false;
}

void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them 
	if (m_iAppID == pCallback->m_nGameID )
	{
		if (k_EResultOK == pCallback->m_eResult)
		{

			if (!inited)
			{
				dirtyStats = true;
				out << "Received stats and achievements from Steam but not init yet\n";
			}
			else
			{
				updateStats();
			}
		}
		else
		{
			out << "RequestStats - failed: " << pCallback->m_eResult << "\n";
		}
	}
}

void CSteamAchievements::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them 
	if (m_iAppID == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			//out << "Stored stats for Steam\n";
		}
		else if (k_EResultInvalidParam == pCallback->m_eResult)
		{
			// One or more stats we set broke a constraint. They've been reverted, 
			// and we should re-iterate the values now to keep in sync. 
			out << "StoreStats - some failed to validate\n";
			// Fake up a callback here so that we re-load the values. 
			UserStatsReceived_t callback;
			callback.m_eResult = k_EResultOK;
			callback.m_nGameID = m_iAppID;
			OnUserStatsReceived(&callback);
		}
		else
		{
			out << "StatsStored - failed: " << pCallback->m_eResult << "\n";
		}
	}
}

void CSteamAchievements::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them 
	if (m_iAppID == pCallback->m_nGameID)
	{
		out << "Stored Achievement for Steam\n";
	}
}

void CSteamAchievements::resetUser()
{
	for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
	{
		Achievement_t &ach = m_pAchievements[iAch];
		ach.m_bAchieved = false;
	}
	for (int iStat = 0; iStat < m_iNumStats; ++iStat)
	{
		Stat_t &stat = m_pStats[iStat];
		stat.m_iValue = 0;
		stat.m_flValue = 0.0f;
	}
	storeStats();
	SteamUserStats()->ResetAllStats(true);
}


void CSteamAchievements::tick()
{
	if (inited && dirtyStats)
	{
		updateStats();
	}
}

void CSteamAchievements::updateStats()
{
	bool b = false;
	out << "Received stats and achievements from Steam\n";
	m_bInitialized = true;
	// load achievements 
	for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
	{
		Achievement_t &ach = m_pAchievements[iAch];
		b = SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
	}

	// load stats 
	for (int iStat = 0; iStat < m_iNumStats; ++iStat)
	{
		Stat_t &stat = m_pStats[iStat];
		switch (stat.m_eStatType)
		{
		case STAT_INT:
			b = SteamUserStats()->GetStat(stat.m_pchID, &stat.m_iValue);
			break;
		case STAT_FLOAT:
		case STAT_AVGRATE:
			b = SteamUserStats()->GetStat(stat.m_pchID, &stat.m_flValue);
			break;
		default:
			break;
		}
	}
	out << "Received stats and achievements from Steam fin\n";
	dirtyStats = false;
}

CSteam::CSteam()
{
	steamInited = SteamAPI_Init();
	achivements = achivements = new CSteamAchievements();
	if (!steamInited)
	{
		out << "Steam init failed\n";
	}
	else
	{
		SteamUtils()->SetOverlayNotificationPosition(k_EPositionBottomLeft);
	}
}

CSteam::~CSteam()
{
	SteamAPI_Shutdown();
	// Delete the SteamAchievements object 
	SAFE_DELETE(achivements);
}

void CSteam::tick()
{
	if (steamInited)
	{
		SteamAPI_RunCallbacks();
		if (achivements)
		{
			achivements->tick();
		}
	}
}

void CSteam::init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats)
{
	// Create the SteamAchievements object if Steam was successfully initialized 
	achivements->init(
		Achievements, NumAchievements,
		Stats, NumStats
	);
}


#endif