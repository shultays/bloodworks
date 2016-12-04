#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
#include "MonsterController.h"
#include "BulletController.h"
#include "MissionController.h"


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

class Bloodworks : public cGame
{
	cTexturedQuadRenderable *bg;
	std::vector<cTexturedQuadRenderable*> fgs;
	Player *player;

	cTextureShr ring;

	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;

	MissionController missionController;
	MonsterController monsterController;
	BulletController bulletController;
	std::vector<Gun*> guns;
	std::vector<Bonus*> bonuses;
	std::vector<Perk*> perks;
	std::vector<Perk*> usedPerks;

	BloodRenderable *bloodRenderable;

	struct Drop
	{
		Gun *gun;
		Bonus *bonus;
		Vec2 pos;
		cRenderable *renderable;
	};

	std::vector<Drop> drops;

	std::unordered_map<std::string, cParticleTemplate*> particles;
	cParticleTemplate *fireParticle;

	struct ExplosionData
	{
		cTexturedQuadRenderable *ringRenderable;
		int id;
		float startTime;
		float maxScale;
		float lastDamageScale;
		int minDamage;
		int maxDamage;
		float scaleSpeed;
		Vec2 pos;
	};
	std::vector<ExplosionData> explosions;

	static int nextUniqueId;
	cParticle *explosionParticles;

	Vec2 mapSize;
	Vec2 mapBegin;
	Vec2 mapEnd;

	Vec2 cameraCenterPos;

	float pauseSlowdown;
	float gamePlaySlowdown;
	float targetGamePlaySlowdown;

	bool paused;

	cPostProcess* pausePostProcess;
protected:
	virtual void render() override;
	virtual void tick() override;
	virtual void init() override;

public:
	Bloodworks() {}
	virtual ~Bloodworks();

	MonsterController* getMonsterController()
	{
		return &monsterController;
	}

	BulletController* getBulletController()
	{
		return &bulletController;
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
	void createGun(const Vec2& pos, int forceIndex = -1);
	void addDrop(const Vec2& position);

	void addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage);
	void createBonus(const Vec2& position, int forceIndex = -1);

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
};