#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
#include "MonsterController.h"
#include "BulletController.h"
#include "MissionController.h"

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
class Bonus;

class Bloodworks : public cGame
{
	cTexturedQuadRenderable *bg;
	Player *player;


	int tickCount;
	int renderCount;
	float lastSetTickTime;
	float lastSetRenderTime;

	MissionController missionController;
	MonsterController monsterController;
	BulletController bulletController;
	std::vector<Gun*> guns;
	std::vector<Bonus*> bonuses;

	BloodRenderable *bloodRenderable;

	struct Drop
	{
		Gun *gun;
		Bonus *bonus;
		Vec2 pos;
		cRenderable *renderable;
	};

	std::vector<Drop> drops;


	cParticleTemplate *particleTemplate;
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
protected:
	virtual void render() override;
	virtual void tick(float dt) override;
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
		return particleTemplate; //todo
	}
	void createGun(const Vec2& pos);
	void addDrop(const Vec2& position);
	const Mat3& getViewMatrix() const;

	void addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage);
	void createBonus(const Vec2& position);

	int getUniqueId()
	{
		return nextUniqueId++;
	}
};