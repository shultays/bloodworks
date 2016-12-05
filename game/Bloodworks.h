#pragma once

#include "cGame.h"
#include "cSharedPtr.h"

enum Depths
{
	BACKGROUND = 0,
	MONSTERS = 1000,
	PLAYER = 2000,
	GAME_OBJECTS = 3000,
	BULLETS = 4000,
	OBJECT_GUI = 5000,
	FOREGROUND = 6000,
	GUI = 7000
};

class cParticle;
class cParticleTemplate;
class cTexturedQuadRenderable;
class Player;
class Monster;
class Bullet;
class Gun;
class MissionController;
class BloodRenderable;
class Bonus;
class Perk;
class cPostProcess;
class cTextRenderable;
class LevelUpPopup;
class ExplosionController;
class DropController;
class MissionController;
class MonsterController;
class BulletController;
class BloodworksLuaWorld;


class Bloodworks : public cGame
{
	Player *player;

	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;

	MissionController *missionController;
	MonsterController *monsterController;
	BulletController *bulletController;
	ExplosionController *explosionController;
	DropController *dropController;
	BloodworksLuaWorld *luaWorld;

	BloodRenderable *bloodRenderable;
	cPostProcess *pausePostProcess;
	LevelUpPopup *levelUpPopup;

	std::vector<Gun*> guns;
	std::vector<Bonus*> bonuses;
	std::vector<Perk*> perks;
	std::vector<Perk*> usedPerks;

	cTexturedQuadRenderable *bg;
	std::vector<cTexturedQuadRenderable*> fgs;

	std::unordered_map<std::string, cParticleTemplate*> particles;

	static int nextUniqueId;

	Vec2 mapSize;
	Vec2 mapBegin;
	Vec2 mapEnd;

	Vec2 cameraCenterPos;

	float pauseSlowdown;
	float gamePlaySlowdown;
	float targetGamePlaySlowdown;

	bool paused;

protected:
	virtual void render() override;
	virtual void tick() override;
	virtual void init() override;

public:
	Bloodworks() {}
	virtual ~Bloodworks();

	MonsterController* getMonsterController() const
	{
		return monsterController;
	}

	BulletController* getBulletController() const
	{
		return bulletController;
	}

	MissionController* getMissionController() const
	{
		return missionController;
	}

	Player* getPlayer()
	{
		return player;
	}

	BloodRenderable* getBloodRenderable();

	cParticleTemplate* getParticleTemplate(const std::string& name)
	{
		return particles[name];
	}

	void addDrop(const Vec2& position);
	void addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage);

	int getUniqueId()
	{
		return nextUniqueId++;
	}
	const Vec2& getMapSize() const
	{
		return mapSize;
	}
	const Vec2& getMapMin() const
	{
		return mapBegin;
	}
	const Vec2& getMapMax() const
	{
		return mapEnd;
	}
	bool isCoorOutside(const Vec2& pos, float radius) const;
	bool isCoorOutside(const Vec2& pos) const;

	void onAddedGunBullet(Gun *gun, Bullet *bullet);

	float getPauseSlowdown() const
	{
		return pauseSlowdown;
	}

	void multiplyGameSpeed(float multiplier);

	void openLevelupPopup();
	void doPause();
	void doUnpause();
	std::vector<Perk*> getAvailablePerks() const;
	void onPerkUsed(Perk *levelupPerks);
	std::vector<Gun*>& getGuns();
	std::vector<Bonus*>& getBonuses();
};