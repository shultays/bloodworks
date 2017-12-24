#pragma once

#include "cSteam.h"
#include "cGlobals.h"


#define _ACH_ID( id ) { id, #id, false } 
Achievement_t g_Achievements[] = { _ACH_ID(ACH_TEST), };
#define _STAT_ID( id,type ) { id, #id, type, 0, 0, 0, 0 }
Stat_t g_Stats[] = { _STAT_ID(STA_TEST, STAT_INT), };


CSteamAchievements::CSteamAchievements(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats)
	: m_iAppID(0)
	, m_bInitialized(false)
	, m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived)
	, m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored)
	, m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
{
	m_iAppID = SteamUtils()->GetAppID();
	m_pAchievements = Achievements;
	m_iNumAchievements = NumAchievements;

	m_pStats = Stats; 
	m_iNumStats = NumStats;

	RequestStats();
}

CSteamAchievements::~CSteamAchievements()
{

}

bool CSteamAchievements::RequestStats() 
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

bool CSteamAchievements::SetAchievement(EAchievements achivement) {
	// Have we received a call back from Steam yet? 
	if (m_bInitialized) 
	{ 
		
		SteamUserStats()->SetAchievement( g_Achievements[ achivement ].m_pchAchievementID );
		return SteamUserStats()->StoreStats(); 
	}
	// If not then we can't set achievements yet 
	return false;
}

bool CSteamAchievements::StoreStats()
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

bool CSteamAchievements::AddStat(EStats stat, int val)
{
	if (m_bInitialized)
	{
		SteamUserStats()->SetStat(g_Stats[stat].m_pchID, g_Stats[stat].m_iValue += val);
		return SteamUserStats()->StoreStats();
	}
	return false;
}

void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t *pCallback) 
{ 
	// we may get callbacks for other games' stats arriving, ignore them 
	if ( m_iAppID == pCallback->m_nGameID ) 
	{ 
		if ( k_EResultOK == pCallback->m_eResult ) 
		{ 
			out << ("Received stats and achievements from Steam\n");
			m_bInitialized = true; 
			// load achievements 
			for ( int iAch = 0; iAch < m_iNumAchievements; ++iAch ) 
			{ 
				Achievement_t &ach = m_pAchievements[iAch]; 
				SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved); 
			} 

			// load stats 
			for ( int iStat = 0; iStat < m_iNumStats; ++iStat ) 
			{ 
				Stat_t &stat = m_pStats[iStat];
				switch (stat.m_eStatType) 
				{ 
				case STAT_INT: 
					SteamUserStats()->GetStat(stat.m_pchID, &stat.m_iValue); 
					break; 
				case STAT_FLOAT: 
				case STAT_AVGRATE: 
					SteamUserStats()->GetStat(stat.m_pchID, &stat.m_flValue);
					break; 
				default: 
					break; 
				} 
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
	if ( m_iAppID == pCallback->m_nGameID ) 
	{ 
		if ( k_EResultOK == pCallback->m_eResult ) 
		{ 
			out << "Stored stats for Steam\n"; 
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
	if ( m_iAppID == pCallback->m_nGameID ) 
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
	StoreStats();
	SteamUserStats()->ResetAllStats(true);
}


CSteam::CSteam()
{
	 bool bRet = SteamAPI_Init(); 
	 // Create the SteamAchievements object if Steam was successfully initialized 
	 if (bRet) 
	 { 
		 achivements = new CSteamAchievements(
			 g_Achievements, sizeof(g_Achievements) / sizeof(g_Achievements[0]),
			 g_Stats, sizeof(g_Stats) / sizeof(g_Stats[0])
		 );
	 }
	 else
	 {
		 achivements = nullptr;
		 out << "Steam init failed\n";
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
	SteamAPI_RunCallbacks();
}
