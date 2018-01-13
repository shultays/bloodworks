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

void CSteamAchievements::init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats, Leaderboard_t *Leaderboards, int NumLeaderboards)
{
	inited = true;
	m_iAppID = SteamUtils()->GetAppID();
	m_pAchievements = Achievements;
	m_iNumAchievements = NumAchievements;

	m_pStats = Stats;
	m_iNumStats = NumStats;

	m_pLeaderboards = Leaderboards;
	m_iNumLeaderboards = NumLeaderboards;

	dirtyStats = false;
	scoreError = false;

	curLeaderboardLoad = 0;

	scoresLoaded = false;
	scoresUploaded = false;

	requestStats();
	requestLeaderboards();
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

void CSteamAchievements::requestLeaderboards()
{
	if (curLeaderboardLoad < m_iNumLeaderboards)
	{
		SteamAPICall_t hSteamAPICall = SteamUserStats()->FindOrCreateLeaderboard(m_pLeaderboards[curLeaderboardLoad].m_pchID, k_ELeaderboardSortMethodAscending, k_ELeaderboardDisplayTypeNumeric);
		m_SteamCallResultCreateLeaderboard.Set(hSteamAPICall, this, &CSteamAchievements::OnFindLeaderboard);
	}
	else
	{
		curLeaderboardLoad = 0;
		requestLeaderboardEntries();
	}
}

void CSteamAchievements::uploadScoresInternal()
{

	if (curLeaderboardLoad < m_iNumLeaderboards)
	{
		SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(m_pLeaderboards[curLeaderboardLoad].steamID, k_ELeaderboardUploadScoreMethodKeepBest, 
			m_pLeaderboards[curLeaderboardLoad].score, NULL, 0);
		m_pLeaderboards[curLeaderboardLoad].lastUploadScore = m_pLeaderboards[curLeaderboardLoad].score;

		m_SteamCallResultUploadScore.Set(hSteamAPICall, this, &CSteamAchievements::OnUploadScore);
	}
	else
	{
		scoresUploaded = true;
	}
}

void CSteamAchievements::OnUploadScore(LeaderboardScoreUploaded_t *pFindLearderboardResult, bool bIOFailure)
{
	if (!bIOFailure && pFindLearderboardResult->m_bSuccess)
	{
		if (pFindLearderboardResult->m_bScoreChanged)
		{
			m_pLeaderboards[curLeaderboardLoad].rank = pFindLearderboardResult->m_nGlobalRankNew;
		}
		m_pLeaderboards[curLeaderboardLoad].count = SteamUserStats()->GetLeaderboardEntryCount(pFindLearderboardResult->m_hSteamLeaderboard);
		if (m_pLeaderboards[curLeaderboardLoad].count < 1)
		{
			m_pLeaderboards[curLeaderboardLoad].count = 1;
		}
	}
	curLeaderboardLoad++;
	uploadScoresInternal();
}

void CSteamAchievements::uploadScores()
{
	scoresUploaded = false;
	curLeaderboardLoad = 0;
	uploadScoresInternal();
}

bool CSteamAchievements::AreScoresReady() const
{
	return scoresLoaded && !scoreError;
}

bool CSteamAchievements::isUploadingScores() const
{
	return !scoresUploaded;
}

void CSteamAchievements::OnFindLeaderboard(LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure)
{
	if (!bIOFailure && pFindLeaderboardResult->m_bLeaderboardFound)
	{
		m_pLeaderboards[curLeaderboardLoad].steamID = pFindLeaderboardResult->m_hSteamLeaderboard;
		m_pLeaderboards[curLeaderboardLoad].count = SteamUserStats()->GetLeaderboardEntryCount(pFindLeaderboardResult->m_hSteamLeaderboard);
		if (m_pLeaderboards[curLeaderboardLoad].count < 1)
		{
			m_pLeaderboards[curLeaderboardLoad].count = 1;
		}
	}
	else
	{
		scoreError = true;
	}

	curLeaderboardLoad++;
	requestLeaderboards();
}

void CSteamAchievements::requestLeaderboardEntries()
{
	if (curLeaderboardLoad < m_iNumLeaderboards)
	{
		SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(m_pLeaderboards[curLeaderboardLoad].steamID, k_ELeaderboardDataRequestGlobalAroundUser,
			0, 0);

		// Register for the async callback
		m_callResultDownloadEntries.Set(hSteamAPICall, this, &CSteamAchievements::OnLeaderboardDownloadedEntries);
	}
	else
	{
		scoresLoaded = true;
		scoresUploaded = true;
	}
}


void CSteamAchievements::OnLeaderboardDownloadedEntries(LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded, bool bIOFailure)
{
	if (pLeaderboardScoresDownloaded->m_cEntryCount > 0 && !bIOFailure)
	{
		LeaderboardEntry_t entry;

		int32 details;

		SteamUserStats()->GetDownloadedLeaderboardEntry(pLeaderboardScoresDownloaded->m_hSteamLeaderboardEntries, 0, &entry, &details, 1);

		m_pLeaderboards[curLeaderboardLoad].score = m_pLeaderboards[curLeaderboardLoad].lastUploadScore = entry.m_nScore;
		m_pLeaderboards[curLeaderboardLoad].rank = entry.m_nGlobalRank;
	}
	else
	{
		m_pLeaderboards[curLeaderboardLoad].score = m_pLeaderboards[curLeaderboardLoad].lastUploadScore = 0;
		m_pLeaderboards[curLeaderboardLoad].rank = m_pLeaderboards[curLeaderboardLoad].count + 1;
	}

	curLeaderboardLoad++;
	requestLeaderboardEntries();
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
	LastEntrySet S("CSteam::tick");
	if (steamInited)
	{
		SteamAPI_RunCallbacks();
		if (achivements)
		{
			achivements->tick();
		}
	}
}

void CSteam::init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats, Leaderboard_t *Leaderboards, int NumLeaderboards)
{
	// Create the SteamAchievements object if Steam was successfully initialized 
	achivements->init(
		Achievements, NumAchievements,
		Stats, NumStats,
		Leaderboards, NumLeaderboards
	);
}


#endif