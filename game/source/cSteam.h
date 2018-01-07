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
public:
	CSteamAchievements();
	void init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats);
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
private:
	void updateStats();
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
	void init(Achievement_t *Achievements, int NumAchievements, Stat_t *Stats, int NumStats);
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