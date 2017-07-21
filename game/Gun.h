#pragma once

#include <string>
#include "cMat.h"
#include "cVec.h"
#include "cResources.h"
#include "sol.h"
#include "json.h"
#include "cSound.h"	
#include "BuffFloat.h"	

class Bloodworks;
class Bullet;
class LaserRenderable;
class Monster;

class Gun
{
	friend class BloodworksLuaWorld;

	Bloodworks *bloodworks;
	std::string name;
	std::string artFolder;
	std::string bulletTexturePath;
	std::string iconPath;
	cTextureShr bulletTexture;
	Vec2 bulletSize;
	Vec2 bulletMeshShift;
	float bulletRadius;
	float bulletSpeed;

	std::string scriptName;
	sol::table scriptTable;
	int id;

	sol::table data;

	Bullet* addBullet();

	float spreadAngle;
	float crosshairDistance;
	IntVec2 damage;

	bool hideSpread;

	bool isTriggered;

	Vec4 shootParticleColor;
	LaserRenderable *laser;

	cSoundSampleWithParams gunShootSound;
	cSoundHandle gunShootSoundHandle;

	cSoundSampleWithParams bulletHitSound;

	cSoundSampleWithParams reloadBeginSound;
	cSoundSampleWithParams reloadEndSound;

	float lastShootSoundTime;

	float lastAmmoReset;
	int currentAmmo;
	int maxAmmo;
	float reloadTime;
	float remainingReload;
	bool reloading;
	bool reloadEnding;

	int buffedMaxAmmo;

	BuffFloat reloadSpeedMultiplier;

	float spawnChance;
	sol::function dynamicSpawnChance;
	float bulletLifeTime;

	float gunShootSoundFadein;
	float gunShootSoundFadeout;
	bool playGunShootSound;
	float gunShootSoundCurVolume;
	bool gunShootSoundContinuous;
public:
	Gun(Bloodworks *bloodworks, nlohmann::json& j);
	~Gun();
	void stop();
	void start();
	void tick(float dt);

	float getMaxCrosshairDistance();
	float getSpreadAngle();
	int getId();
	sol::table& getScriptTable();
	const std::string& getName()
	{
		return name;
	}
	const std::string& getIconPath() const;
	bool spreadVisible() const;
	void setTriggered(bool param1);
	const Vec4& getShootingParticleColor() const;

	bool isLaser() const
	{
		return laser != nullptr;
	}

	void updateLaser(const Vec2& pos, float angle);

	int getRandomDamage() const
	{
		return randInt(damage[0], damage[1]);
	}
	void reset();
	void onBulletHit(Bullet *bullet, Monster* monster);

	bool hasAmmo() const
	{
		return maxAmmo == 0 || currentAmmo > 0;
	}

	void addAmmo();
	void consumeAmmo();
	void reload();
	int getCurrentAmmo() const;
	bool isReloading() const;
	float getReloadPercentage() const;

	int getMaxAmmo();
	float getSpawnChance();
	float getBulletLifeTime() const;
};