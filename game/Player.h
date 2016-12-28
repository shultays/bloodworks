#pragma once

#include "cMat.h"
#include "cVec.h"
#include "sol.h"

class Bloodworks;
class Gun;
class cTextRenderable;
class cRenderable;
class cRenderableGroup;
class cAnimatedTexturedQuadRenderable;

class Player
{
	friend class BloodworksLuaWorld;
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
	Vec2 gunPos;
	Gun *gun;
	Gun *secondaryGun;
	Vec2 aimDir;
	Vec2 moveDir;
	Vec2 moveSpeedDir;
	float aimAngle;
	float oldSpreadAngle;
	int hitPoints;
	int maxHitPoints;
	cTextRenderable *healthRenderable;

	cAnimatedTexturedQuadRenderable *shootRenderable;

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
	bool visible;

	int calculateExperienceForLevel(int level);

	sol::table data;

	void updateHitPoints();
public:
	Player(Bloodworks *bloodworks);
	~Player();
	void tick();

	const Vec2& getPosition() const
	{
		return pos;
	}

	const Vec2& getAimDir() const
	{
		return aimDir;
	}

	void setGun(Gun* gun);
	Gun* getGun();

	void setSecondaryGun(Gun* gun);
	Gun* getSecondaryGun();

	int doDamage(int damage);
	int doDamageWithParams(int damage, sol::table& params);
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
	void doHeal(int hp);
	int getLevel() const;
	void playShootAnimation();
	const Vec2& getGunPos() const;
	bool isVisible() const
	{
		return visible;
	}
	void setVisible(bool visible);
	void reset();
};