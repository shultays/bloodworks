#pragma once

#define HAS_BLOODWORKS_CHEAT

#ifdef HAS_BLOODWORKS_CHEAT

class Bloodworks;
class Monster;
class BloodworksCheat
{
	Bloodworks *bloodworks;
	bool showFPS;
	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;
	bool moveMonsters;
	int slowdownBuff;
public:
	static BloodworksCheat *instance;
	BloodworksCheat(Bloodworks *bloodworks);
	void onTick();
	void onMonsterTick(Monster *monster);
	void onRender();
	void onLoadMission();
	void onMonsterPreTick(Monster* monster);
	void onInit();
};

#endif