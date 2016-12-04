#pragma once

#include "cMat.h"
#include "cVec.h"
#include "cRenderable.h"

class Bloodworks;
class Gun;

class Player
{
	friend class Bloodworks;

	Vec2 pos;
	Vec2 oldPos;
	Vec2 oldMoveAmount;

	float moveAngle;
	float moveSpeed;

	Bloodworks *bloodworks;
	cRenderableGroup *renderable;
	cRenderable *crosshair;
	cRenderable *spread;
	Vec2 crosshairPos;
	Gun *gun;
	Vec2 aimDir;
	Vec2 moveDir;
	Vec2 moveSpeedDir;
	float aimAngle;
	float oldSpreadAngle;
	int hitPoints;
	cTextRenderable *healthRenderable;

	float slowdownAmount;
	float slowdownDuration;

	float bulletSpeedMult;
	float shootSpeedMult;
	float moveSpeedMult;

	bool slowdownOnHit;

	cRenderable *healthBarActive, *healthBarBG, *healthBarFG;
	Vec2 barSize;

	int level;
	int experience;
	int experienceForNextLevel;

	int calculateExperienceForLevel(int level);
public:
	Player(Bloodworks *bloodworks);
	~Player();
	void tick();

	const Vec2& getPos() const
	{
		return pos;
	}

	const Vec2& getAimDir() const
	{
		return aimDir;
	}

	void setGun(Gun* gun);
	Gun* getGun();

	void doDamage(int damage);
	void slowdown(float slowdownAmount, float slowdownDuration);
	const Vec2& getCrosshairPos() const 
	{
		return crosshairPos;
	}

	float getBulletSpeedMultiplier()
	{
		return bulletSpeedMult;
	}

	void gainExperience(int experience);
	void doLevelup();
private:
	void updateHitPoints();
};