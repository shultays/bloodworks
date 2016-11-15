#pragma once

#include "cGame.h"
#include "cSharedPtr.h"
#include "MonsterController.h"
#include "BulletController.h"
#include "MissionController.h"

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

	void createGun(const Vec2& pos);
	void addDrop(const Vec2& position);
private:
	void createBonus(const Vec2& position);
public:
	const Mat3& getViewMatrix() const;
};