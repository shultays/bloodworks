#pragma once

#define HAS_BLOODWORKS_DEBUG

#ifdef HAS_BLOODWORKS_DEBUG

class Bloodworks;
class Monster;
class BloodworksDebug
{
	Bloodworks *bloodworks;
	bool showFPS;
	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;
	bool moveMonsters;
public:
	static BloodworksDebug *instance;
	BloodworksDebug(Bloodworks *bloodworks);
	void onTick();
	void onMonsterTick(Monster *monster);
	void onRender();
	void onLoadMission();
	void onMonsterPreTick(Monster* monster);
	void onInit();
};

#endif