#pragma once

#include "cTools.h"
#include "sol.h"
#include "cResources.h"
#include "BuffFloat.h"
#include "DirentHelper.h"

class Bloodworks;
class Gun;
class cTextRenderable;
class cRenderable;
class cRenderableContainer;
class cAnimatedTexturedQuadRenderable;
class cTexturedQuadRenderable;
class cRenderableWithShader;

class PlayerTemplate
{
	friend class Player;

	std::string name;

	int startHitPoint;

	float maxSpeed;
	float maxRotateSpeed;

	float bulletRadius;
	float collisionRadius;

	float acceleration;
	float decceleration;

	float minRotationSpeed;
	float maxRotationSpeed;

	float playerScale;

	std::string body;
	Vec2 shift;
	Vec2 scale;

	std::string gunFire;

	Vec2 gunFireShift;
	Vec2 gunFireScale;

	Vec2 bulletStartShift;

	float legSpeed;

	float legMinScale;
	float legMaxScale;

	float legMaxRotate;

	std::string leftLegFront;
	std::string rightLegFront;
	std::string leftLegBack;
	std::string rightLegBack;

public:
	PlayerTemplate(nlohmann::json& j, const DirentHelper::File& file);

	const std::string& getName() const 
	{
		return name;
	}
};

class Player
{
	friend class BloodworksLuaWorld;
	Vec2 pos;
	Vec2 oldPos;
	Vec2 oldMoveAmount;

	float moveAngle;
	float moveSpeed;

	Bloodworks *bloodworks;
	cRenderableContainer *renderable;
	cTexturedQuadRenderable *body;

	cRenderableContainer *legRenderable;
	Mat3 bodyMat;
	cTexturedQuadRenderable *leftLegFront;
	cTexturedQuadRenderable *leftLegBack;

	cTexturedQuadRenderable *rightLegFront;
	cTexturedQuadRenderable *rightLegBack;

	cRenderable *crosshair;
	cRenderableWithShader *ammo;

	cRenderableContainer *primaryRenderable;
	cRenderableWithShader *primaryAmmo;
	cTexturedQuadRenderable *primaryIcon;

	cRenderableContainer *secondaryRenderable;
	cRenderableWithShader *secondaryAmmo;
	cTexturedQuadRenderable *secondaryIcon;
	cRenderable *spread;
	Vec2 crosshairPos;
	float crosshairDistance;
	Vec2 gunPos;
	Gun *gun;
	Gun *secondaryGun;
	Vec2 aimDir;
	Vec2 moveDir;
	Vec2 moveVelocity;
	float aimAngle;
	float oldSpreadAngle;
	int hitPoints;
	int maxHitPoints;

	float legTimer;

	cTextRenderable *healthRenderable;
	cAnimatedTexturedQuadRenderable *shootRenderable;

	BuffFloat maxSpeed;
	BuffFloat maxRotateSpeed;
	BuffFloat monsterExperienceMultiplier;
	BuffFloat damageMultiplier;
	BuffFloat shootSpeedMultiplier;
	BuffFloat bulletSpeedMultiplier;
	BuffFloat reloadSpeedMultiplier;
	BuffFloat globalMonsterSpeedMultiplier;
	BuffFloat clipCountMultiplier;
	BuffFloat gunSpreadMultiplier;
	BuffFloat accelerationMultiplier;
	BuffFloat scaleMultiplier;
	BuffVec4 colorMultiplier;

	cRenderable *healthBarActive, *healthBarBG, *healthBarFG;
	Vec2 barSize, scaledBarSize;

	cRenderable *experienceBarActive, *experienceBarBG, *experienceBarFG;
	Vec2 scaledExpBarSize;

	cTextRenderable *scoreText;

	float lastScale;
	int score;
	int level;
	int experience;
	int experienceForNextLevel;
	bool visible;
	bool isDead;

	sol::table data;

	int calculateExperienceForLevel(int level);
	void updateHitPoints();

	cVector<cSoundSampleShr> hitSounds;
	cVector<cSoundSampleShr> killSounds;

	int currentAmmoUniformIndex;
	int maxAmmoUniformIndex;
	int reloadingUniformIndex;
	float reloadAlpha;
	float secondaryReloadAlpha;

	float joystickCheckTimer;

	void checkInput(bool& moving, float& wantedAngle);
	void updateExperience();

	struct OldGun
	{
		float timeToTick;
		Gun* gun;
	};
	cVector<OldGun> oldGuns;

	struct Knockback
	{
		Vec2 speed;
		float duration;
	};
	cVector<Knockback> knockbacks;

	const PlayerTemplate *playerTemplate;
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
	float getAimAngle() const
	{
		return aimAngle;
	}

	void setGun(Gun* gun);
	Gun* getGun();

	void setSecondaryGun(Gun* gun);
	Gun* getSecondaryGun();

	void addScore(int score);
	int getScore() const
	{
		return score;
	}

	bool isActive() const 
	{
		return isDead == false;
	}
	int doDamage(int damage, float angle);
	int doDamageWithArgs(int damage, float angle, sol::table& params);
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
	void setScore(int score);
	int getMaxHitpoints() const
	{
		return maxHitPoints;
	}
	int getHitpoints() const
	{
		return hitPoints;
	}
	void setMaxHitpoints(int maxHitPoints)
	{
		this->maxHitPoints = maxHitPoints;
		if (hitPoints > maxHitPoints)
		{
			hitPoints = maxHitPoints;
		}
		updateHitPoints();
	}
	void setHitpoints(int hitPoints)
	{
		this->hitPoints = hitPoints;
		if (hitPoints > maxHitPoints)
		{
			hitPoints = maxHitPoints;
		}
		updateHitPoints();
	}
	void killSelf();
	float getBulletRadius() const;
	float getCollisionRadius() const;
	void addKnockback(const Vec2& speed, float duration);
	float getBulletSpreadMultiplier();
	void buildBody(const PlayerTemplate* playerTemplate);
};