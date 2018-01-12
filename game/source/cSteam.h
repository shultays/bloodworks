#pragma once

#include "steam_api.h" 

#include "cdefines.h" 

#ifdef HAS_STEAM

struct Achievement_t 
{ 
	int m_eAchievementID; 
	const char *m_pchAchievementID; 
	bool m_bAchieved; 
};


enum EStatTypes 
{
	STAT_INT = 0, 
	STAT_FLOAT = 1, 
	STAT_AVGRATE = 2, 
}; 

struct Stat_t 
{
	int m_ID;
	const char *m_pchID;
	EStatTypes m_eStatType; 
	int m_iValue; 
	float m_flValue; 
	float m_flAvgNumerator; 
	float m_flAvgDenominator; 
};

struct Leaderboard_t
{
	int m_ID;
	const char *m_pchID;
	int score;

	int rank;
	int count;
	int lastUploadScore;
	SteamLeaderboard_t steamID;
};

class CSteamAchievements
{
private: 
	int64 m_iAppID; // Our current AppID
	Achievement_t *m_pAchievements; // Achievements data
	int m_iNumAchievements; // The number of Achievements
	bool m_bInitialized; // Have we called Request stats and received the callback?

	Stat_t *m_pStats; // Stats data 
	int m_iNumStats; // The number of Stats

	bool dirtyStats;
	bool inited;

	Leaderboard_t *m_pLeaderboards; // Stats data 
	int m_iNumLeaderboards; // The number of Stats

	bool scoresUploaded;
	bool scoresLoaded;
	bool scoreError;
public:
	CSteamAchievements();
	void init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats, Leaderboard_t *Leaderboards, int NumLeaderboards);
	~CSteamAchievements();
	bool requestStats();
	bool setAchievement(const char* ID);
	bool storeStats();

	bool setStat(const char* ID, int val);

	STEAM_CALLBACK(CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
	STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
	STEAM_CALLBACK(CSteamAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);

	void resetUser();
	void tick();

	void OnFindLeaderboard(LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure);
	CCallResult<CSteamAchievements, LeaderboardFindResult_t> m_SteamCallResultCreateLeaderboard;

	void OnLeaderboardDownloadedEntries(LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded, bool bIOFailure);
	CCallResult<CSteamAchievements, LeaderboardScoresDownloaded_t> m_callResultDownloadEntries;

	void OnUploadScore(LeaderboardScoreUploaded_t *pFindLearderboardResult, bool bIOFailure);
	CCallResult<CSteamAchievements, LeaderboardScoreUploaded_t> m_SteamCallResultUploadScore;

	void uploadScores();
	bool AreScoresReady() const;
	bool isUploadingScores() const;
private:
	void uploadScoresInternal();
	void updateStats();
	void requestLeaderboards();
	int curLeaderboardLoad;
	void requestLeaderboardEntries();
};

class CSteam
{
private:
	CSteamAchievements* achivements;

	bool steamInited;
public:
	CSteam();
	~CSteam();

	void tick();
	void init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats, Leaderboard_t *Leaderboards, int NumLeaderboards);
	CSteamAchievements* getAchievements() const
	{
		return achivements;
	}
	bool isInited() const
	{
		return steamInited;
	}

};

#endif