#pragma once

#define HAS_BLOODWORKS_CHEATS

#ifdef HAS_BLOODWORKS_CHEATS

class Bloodworks;
class Monster;
class BloodworksCheats
{
	Bloodworks *bloodworks;
	bool showFPS;
	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;
	bool moveMonsters;
	int slowdownBuff;
	bool hasCheats;
public:
	static BloodworksCheats *instance;
	BloodworksCheats(Bloodworks *bloodworks);
	~BloodworksCheats();
	void onTick();
	void onMonsterTick(Monster *monster);
	void onRender();
	void onLoadMission();
	void onMonsterPreTick(Monster* monster);
	void onInit();
};

#endif