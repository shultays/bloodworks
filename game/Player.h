#pragma once

#include "cMat.h"
#include "cVec.h"
#include "sol.h"
#include "cResources.h"
#include "BuffFloat.h"

class Bloodworks;
class Gun;
class cTextRenderable;
class cRenderable;
class cRenderableGroup;
class cAnimatedTexturedQuadRenderable;
class cRenderableWithShader;

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
	cRenderableWithShader *ammo;
	cRenderable *spread;
	Vec2 crosshairPos;
	float crosshairDistance;
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

	BuffFloat maxSpeed;
	BuffFloat monsterExperienceMultiplier;
	BuffFloat damageMultiplier;
	BuffFloat shootSpeedMultiplier;
	BuffFloat bulletSpeedMultiplier;
	BuffFloat reloadSpeedMultiplier;
	BuffFloat globalMonsterSpeedMultiplier;
	BuffFloat clipCountMultiplier;
	BuffFloat gunSpreadMultiplier;

	cRenderable *healthBarActive, *healthBarBG, *healthBarFG;
	Vec2 barSize, scaledBarSize;

	cRenderable *experienceBarActive, *experienceBarBG, *experienceBarFG;
	Vec2 scaledExpBarSize;

	int level;
	int experience;
	int experienceForNextLevel;
	bool visible;
	bool isDead;

	sol::table data;

	int calculateExperienceForLevel(int level);
	void updateHitPoints();
	void killSelf();

	std::vector<cSoundSampleShr> hitSounds;
	std::vector<cSoundSampleShr> killSounds;

	int currentAmmoUniformIndex;
	int maxAmmoUniformIndex;
	int reloadingUniformIndex;
	float reloadAlpha;

	float joystickCheckTimer;

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

	bool isActive() const 
	{
		return isDead == false;
	}
	int doDamage(int damage);
	int doDamageWithParams(int damage, sol::table& params);
	const Vec2& getCrosshairPos() const 
	{
		return crosshairPos;
	}

	float getBulletSpeedMultiplier()
	{
		return bulletSpeedMultiplier.getBuffedValue();
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
	float getMonsterExperienceMultiplier();
	float getDamageMultiplier();
	void resize();
	float getReloadSpeedMultiplier();
	float getGlobalMonsterSpeedMultiplier();
	float getClipCountMultiplier();
	int getBuffedClipSize(int clipSize);
private:
	void checkInput(bool& moving, float& wantedAngle);
	void updateExperience();
};