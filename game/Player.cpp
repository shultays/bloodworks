#include "Player.h"
#include "Bloodworks.h"
#include "Bullet.h"
#include "Gun.h"
#include "cFont.h"
#include "cRenderable.h"
#include "cAnimatedRenderable.h"
#include "DirentHelper.h"

#include <sstream>

const float healthPos = -50.0f;
const float expPos = -62.0f;
const float expPosX = -0.5f;

int Player::calculateExperienceForLevel(int level)
{
	return (level - 1) * 150 + (level - 1)  * (level - 1) * 20 + 10;
}

Player::Player(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	Mat3 mat = Mat3::identity();
	mat.scaleBy(15.7f, 22.9f);
	mat.translateBy(0.0f, 5.0f);

	renderable = new cRenderableGroup(bloodworks);
	cTexturedQuadRenderable *body = new cTexturedQuadRenderable(bloodworks, "resources/assault/body.png", "resources/default");
	body->setWorldMatrix(mat);
	renderable->addRenderable(body);

	cTexturedQuadRenderable *hands = new cTexturedQuadRenderable(bloodworks, "resources/assault/hands.png", "resources/default");
	hands->setWorldMatrix(mat);
	renderable->addRenderable(hands);

	healthRenderable = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), "", 10);
	healthRenderable->setTextAllignment(TextAlignment::center);
	healthRenderable->setWorldMatrix(Mat3::identity());
	bloodworks->addRenderable(healthRenderable, OBJECT_GUI);

	renderable->setWorldMatrix(Mat3::identity());
	bloodworks->addRenderable(renderable, PLAYER);

	crosshair = new cTexturedQuadRenderable(bloodworks, "resources/crosshair.png", "resources/default");
	crosshair->setWorldMatrix(Mat3::scaleMatrix(40.0f).translateBy(0.0f, 50.0f));
	bloodworks->addRenderable(crosshair, GUI + 100);

	ammo = new cTexturedQuadRenderable(bloodworks, "resources/reload_ring.png", "resources/reload");
	ammo->setWorldMatrix(Mat3::scaleMatrix(40.0f).translateBy(0.0f, 50.0f));
	bloodworks->addRenderable(ammo, GUI + 100);

	spread = new cTexturedQuadRenderable(bloodworks, "resources/crosshair_spread.png", "resources/default");
	spread->setWorldMatrix(Mat3::scaleMatrix(20.0f));
	bloodworks->addRenderable(spread, GUI + 101);

	barSize = Vec2(256.0f, 32.0f);
	barSize *= 0.4f;
	barSize.x = round(barSize.x);
	barSize.y = round(barSize.y);

	healthBarBG = new cTexturedQuadRenderable(bloodworks, "resources/assault/bar_bg.png", "resources/default");
	healthBarBG->setAlignment(RenderableAlignment::top);
	bloodworks->addRenderable(healthBarBG, GUI + 10);

	healthBarActive = new cTexturedQuadRenderable(bloodworks, "resources/assault/bar_active.png", "resources/default");
	healthBarActive->setAlignment(RenderableAlignment::top);
	healthBarActive->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.6f));
	bloodworks->addRenderable(healthBarActive, GUI + 11);

	healthBarFG = new cTexturedQuadRenderable(bloodworks, "resources/assault/bar_fg.png", "resources/default");
	healthBarFG->setAlignment(RenderableAlignment::top);
	bloodworks->addRenderable(healthBarFG, GUI + 17);

	experienceBarBG = new cTexturedQuadRenderable(bloodworks, "resources/assault/exp_bar_bg.png", "resources/default");
	experienceBarBG->setAlignment(RenderableAlignment::top);
	bloodworks->addRenderable(experienceBarBG, GUI + 12);
	experienceBarBG->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.5f));

	experienceBarActive = new cTexturedQuadRenderable(bloodworks, "resources/assault/exp_bar_active.png", "resources/default");
	experienceBarActive->setAlignment(RenderableAlignment::top);
	experienceBarActive->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.6f));
	bloodworks->addRenderable(experienceBarActive, GUI + 13);
	experienceBarActive->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.5f));

	experienceBarFG = new cTexturedQuadRenderable(bloodworks, "resources/assault/exp_bar_fg.png", "resources/default");
	experienceBarFG->setAlignment(RenderableAlignment::top);
	bloodworks->addRenderable(experienceBarFG, GUI + 18);
	experienceBarFG->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.5f));


	shootRenderable = new cAnimatedTexturedQuadRenderable(bloodworks, "resources/default");
	shootRenderable->addAnimation(cAnimatedTexturedQuadRenderable::AnimationData());
	shootRenderable->addAnimation(getAnimationData("resources/assault/gun_fire/data.json"));
	shootRenderable->setWorldMatrix(Mat3::scaleMatrix(7.0f).rotateBy(-pi_d2).translateBy(4.0f, 32.0f));
	renderable->addRenderable(shootRenderable);

	resize();
	DirentHelper::Folder f("resources/sounds/player_hit");

	std::function<void(DirentHelper::File&)> func = [&](DirentHelper::File &file)
	{
		if (file.file.find("hit") != std::string::npos)
		{
			hitSounds.push_back(resources.getSoundSample(file.folder + file.file));
		}
		if (file.file.find("die") != std::string::npos)
		{
			killSounds.push_back(resources.getSoundSample(file.folder + file.file));
		}
	};

	f.runOnEachFile(func);

	currentAmmoUniformIndex = ammo->addUniformFloat("uCurrentAmmo", 0.0f);
	maxAmmoUniformIndex = ammo->addUniformInt("uMaxAmmo", 0);
	reloadingUniformIndex = ammo->addUniformFloat("uReloading", 0.0f);

	reset();
}

Player::~Player()
{
	SAFE_DELETE(crosshair);
	SAFE_DELETE(ammo);
	SAFE_DELETE(renderable);
	SAFE_DELETE(spread);
	SAFE_DELETE(healthRenderable);

	SAFE_DELETE(healthBarBG);
	SAFE_DELETE(healthBarActive);
	SAFE_DELETE(healthBarFG);

	SAFE_DELETE(experienceBarBG);
	SAFE_DELETE(experienceBarActive);
	SAFE_DELETE(experienceBarFG);

	for (auto& s : hitSounds)
	{
		s = nullptr;
	}
	hitSounds.clear();
	for (auto& s : killSounds)
	{
		s = nullptr;
	}
	killSounds.clear();
}

void Player::tick()
{
	if (visible == false)
	{
		return;
	}

	if (input.isKeyPressed(key_g))
	{
		if (maxHitPoints == 100)
		{
			maxHitPoints = hitPoints = 100000000;
		}
		else
		{
			maxHitPoints = hitPoints = 100;
		}
		updateHitPoints();
	}
	reloadSpeedMultiplier.tick();
	globalMonsterSpeedMultiplier.tick();
	oldPos = pos;

	float wantedAngle = moveAngle;

	bool moving = false;
	checkInput(moving, wantedAngle);
	const float acceleration = 800.0f;
	const float decceleration = 1000.0f;

	float dt = timer.getDt();
	float currentMaxSpeed = maxSpeed;

	if (slowdownAmount > 0.0f && input.isMouseVisible() == false)
	{
		currentMaxSpeed *= 1.0f - slowdownAmount;
		slowdownDuration -= dt;
		if (slowdownDuration < 0.0f)
		{
			slowdownAmount -= dt * 3;
		}
	}

	float minRotation = pi * 2.0f;
	float maxRotation = pi * 10.0f;

	if (moving)
	{
		if (wantedAngle != moveAngle)
		{
			float diff = angleDiff(moveAngle, wantedAngle);
			float rotation = lerp(maxRotation, minRotation, moveSpeed / currentMaxSpeed) * dt;
			if (diff > pi * 0.99f || diff < -pi * 0.99f)
			{
				if (moveSpeed > decceleration * dt)
				{
					moveSpeed -= decceleration * dt;
				}
				if (moveSpeed <= 0.0f)
				{
					moveSpeed = -moveSpeed;
					moveAngle = wantedAngle;
				}
				moving = false;
			}
			else if (diff < -rotation)
			{
				moveAngle += rotation;
			}
			else if (diff > rotation)
			{
				moveAngle -= rotation;
			}
			else
			{
				moveAngle = wantedAngle;
			}
		}
	}

	if (moving)
	{
		moveSpeed += acceleration * dt;
		moveSpeed = min(moveSpeed, currentMaxSpeed);
	}
	else
	{
		moveSpeed -= decceleration * dt;
		moveSpeed = max(moveSpeed, 0.0f);
	}

	moveDir = Vec2::fromAngle(moveAngle);
	moveSpeedDir = moveDir * moveSpeed * moveSpeedMult;

	Vec2 moveAmount = moveSpeedDir * dt;
	Vec2 newPos = pos + moveAmount;
	oldMoveAmount = moveAmount;
	Vec2 boundaryMin = bloodworks->getMapMin() + 20.0f;
	Vec2 boundaryMax = bloodworks->getMapMax() - 20.0f;
	float boundaryAmount = 40.0f;

	if (newPos.x < boundaryMin.x && moveAmount.x < 0.0f)
	{
		moveAmount.x = moveAmount.x * max(0.0f, (newPos.x + boundaryAmount - boundaryMin.x) / boundaryAmount);
	}
	else if (newPos.x > boundaryMax.x && moveAmount.x > 0.0f)
	{
		moveAmount.x = moveAmount.x * max(0.0f, (boundaryMax.x + boundaryAmount - newPos.x) / boundaryAmount);
	}

	if (newPos.y < boundaryMin.y && moveAmount.y < 0.0f)
	{
		moveAmount.y = moveAmount.y * max(0.0f, (newPos.y + boundaryAmount - boundaryMin.y) / boundaryAmount);
	}
	else if (newPos.y > boundaryMax.y && moveAmount.y > 0.0f)
	{
		moveAmount.y = moveAmount.y * max(0.0f, (boundaryMax.y + boundaryAmount - newPos.y) / boundaryAmount);
	}

	pos = pos + moveAmount;
	bool joystickUsed = false;
	if (input.hasJoyStick())
	{
		Vec2 joystick = input.getJoystickAxisPos2();
		float length = joystick.safeNormalize();

		if (joystickCheckTimer >= 0.0f)
		{
			joystickCheckTimer -= dt;
		}
		if (length > 0.0001f || joystickCheckTimer > 0.0f)
		{
			if (length < 0.0001f)
			{
				joystick = aimDir;
			}
			else
			{
				joystickCheckTimer = 0.5f;
				joystickUsed = true;
			}
			length = 0.2f + length * 0.8f;
			clamp(length, 0.0f, 1.0f);
			crosshairPos = lerp(crosshairPos, joystick * min(300.0f, gun->getMaxCrosshairDistance()) * length, timer.getDt() * 10);
		}

	}

	if (joystickUsed == false)
	{
		if (bloodworks->isPaused() == false)
		{
			crosshairPos += input.getDeltaMousePos() * bloodworks->getPauseSlowdown();
		}

		float maxCrosshairDistance = gun ? gun->getMaxCrosshairDistance() : 400.0f;
		float lengthSquared = crosshairPos.lengthSquared();
		if (lengthSquared > maxCrosshairDistance * maxCrosshairDistance)
		{
			crosshairPos = crosshairPos.normalized() * maxCrosshairDistance;
		}
	}
	
	crosshairDistance = 0.0f;

	if (crosshairPos.lengthSquared() > 0.01f)
	{
		aimDir = crosshairPos;
		crosshairDistance = aimDir.normalize();
		aimAngle = aimDir.toAngle();
	}
	if (gun && gun->getMaxAmmo() > 0)
	{
		if (gun->isReloading())
		{
			ammo->setUniform(currentAmmoUniformIndex, gun->getMaxAmmo() * gun->getReloadPercentage());
		}
		else
		{
			ammo->setUniform(currentAmmoUniformIndex, (float)gun->getCurrentAmmo());
		}
		ammo->setUniform(maxAmmoUniformIndex, gun->getMaxAmmo());

		if (gun->isReloading())
		{
			reloadAlpha = 1.0f;
		}
		else
		{
			reloadAlpha -= dt * 6.0f;
			if (reloadAlpha < 0.0f)
			{
				reloadAlpha = 0.0f;
			}
		}

		ammo->setUniform(reloadingUniformIndex, reloadAlpha);
		ammo->setVisible(true);
	}
	else
	{
		ammo->setVisible(false);
	}

	if (gun && gun->spreadVisible())
	{
		float newSpreadAngle = gun->getSpreadAngle();
		const float maxAngleChange = 1.0f * dt;
		if (newSpreadAngle > oldSpreadAngle)
		{
			float spreadAngleDiff = newSpreadAngle - oldSpreadAngle;

			if (spreadAngleDiff > maxAngleChange)
			{
				newSpreadAngle = oldSpreadAngle + maxAngleChange;
			}
		}
		else
		{
			float spreadAngleDiff = newSpreadAngle - oldSpreadAngle;

			if (spreadAngleDiff < -maxAngleChange)
			{
				newSpreadAngle = oldSpreadAngle - maxAngleChange;
			}
		}
		oldSpreadAngle = newSpreadAngle;

		float scale = sin(oldSpreadAngle) * crosshairDistance + 10.0f;
		spread->setWorldMatrix(Mat3::scaleMatrix(scale).translateBy(pos + crosshairPos));
		spread->setColor(Vec4(1.0f, 1.0f, 1.0f, clamped(oldSpreadAngle * 4.0f, 0.0f, 0.4f)));

		spread->setVisible(true);
	}
	else
	{
		spread->setVisible(false);
	}

	crosshair->setWorldMatrix(Mat3::scaleMatrix(14.0f).translateBy(pos + crosshairPos));
	crosshair->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.7f));

	ammo->setWorldMatrix(Mat3::scaleMatrix(22.0f).translateBy(pos + crosshairPos));
	ammo->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.7f));

	healthRenderable->setWorldMatrix(Mat3::translationMatrix(pos + Vec2(0.0f, 30.0f)));

	healthRenderable->setVisible(input.isKeyDown(key_f6));
	gunPos = pos + aimDir * 22.0f - aimDir.sideVec() * 4.0f;

	Mat3 mat = Mat3::identity();
	mat.rotateBy(pi_d2 - aimAngle);
	mat.translateBy(pos);
	renderable->setWorldMatrix(mat);

	if (gun)
	{
		bool trigerred = bloodworks->getPauseSlowdown() > 0.5f && (input.isKeyDown(mouse_button_left) || (input.hasJoyStick() && input.isKeyDown(joystick_0_button_rightshoulder)));
		gun->setTriggered(trigerred);
		if (trigerred && gun->isLaser())
		{
			gun->updateLaser(gunPos, -aimAngle);
		}
		gun->tick(dt);
	}

	if (secondaryGun)
	{
		secondaryGun->setTriggered(input.isKeyDown(mouse_button_right));
		secondaryGun->tick(dt);
	}

	if (input.isKeyPressed(key_q))
	{
		killSelf();
	}
}

void Player::setGun(Gun *gun)
{
	if (this->gun)
	{
		this->gun->stop();
	}

	this->gun = gun;

	if (this->gun)
	{
		shootRenderable->setColor(this->gun->getShootingParticleColor());
		this->gun->start();
	}
}

void Player::setSecondaryGun(Gun *gun)
{
	if (this->secondaryGun)
	{
		this->secondaryGun->stop();
	}

	this->secondaryGun = gun;

	if (this->secondaryGun)
	{
		this->secondaryGun->start();
	}
}

Gun* Player::getGun()
{
	return gun;
}

Gun* Player::getSecondaryGun()
{
	return secondaryGun;
}

int Player::doDamage(int damage)
{
	return doDamageWithParams(damage, lua.create_table());
}

int Player::doDamageWithParams(int damage, sol::table& params)
{
	damage = bloodworks->onPlayerDamaged(damage, params);
	if (damage > 0)
	{
		hitPoints -= damage;
		if (hitPoints <= 0)
		{
			hitPoints = 0;
			killSelf();
			killSounds[randInt((int)killSounds.size())]->play();
		}
		else
		{
			hitSounds[randInt((int)hitSounds.size())]->play();
		}
		updateHitPoints();
	}
	return damage;
}

void Player::slowdown(float slowdownAmount, float slowdownDuration)
{
	this->slowdownAmount = slowdownAmount;
	this->slowdownDuration = slowdownDuration;
}

void Player::updateHitPoints()
{
	std::stringstream ss;
	ss << hitPoints;
	healthRenderable->setText(ss.str());
	float scale = (scaledBarSize.x - 9.0f) * (hitPoints / (float)maxHitPoints);
	if (hitPoints > 1 && bloodworks->isMissionLoaded())
	{
		healthBarActive->setVisible(true);
		healthBarActive->setWorldMatrix(Mat3::scaleMatrix(scale, scaledBarSize.y - 5.0f / bloodworks->getCameraZoom()).translateBy(0.0f, healthPos / bloodworks->getCameraZoom()));
	}
	else
	{
		healthBarActive->setVisible(false);
	}
}

void Player::gainExperience(int e)
{
	this->experience += e;
	if (experience >= experienceForNextLevel && bloodworks->isLevelUpPopupVisible() == false)
	{
		experience -= experienceForNextLevel;
		doLevelup();
	}
	updateExperience();
}

void Player::doLevelup()
{
	level++;
	experienceForNextLevel = calculateExperienceForLevel(level + 1);
	bloodworks->openLevelupPopup();
	updateExperience();
}

void Player::doHeal(int hp)
{
	hitPoints += hp;
	if (hitPoints > maxHitPoints)
	{
		hitPoints = maxHitPoints;
	}
	updateHitPoints();
}

int Player::getLevel() const
{
	return level;
}

void Player::playShootAnimation()
{
	shootRenderable->playAnimation(1);
}

const Vec2& Player::getGunPos() const
{
	return gunPos;
}

void Player::setVisible(bool visible)
{
	this->visible = visible;

	ammo->setVisible(visible);
	crosshair->setVisible(visible);
	renderable->setVisible(visible);
	spread->setVisible(visible);
	healthRenderable->setVisible(visible);

	healthBarBG->setVisible(visible);
	healthBarActive->setVisible(visible);
	healthBarFG->setVisible(visible);

	experienceBarBG->setVisible(visible);
	experienceBarActive->setVisible(visible);
	experienceBarFG->setVisible(visible);
	updateHitPoints();
	updateExperience();
}

void Player::reset()
{
	reloadAlpha = 0.0f;
	maxSpeed = 150.0f;
	damageMult = monsterExperienceMult = moveSpeedMult = shootSpeedMult = bulletSpeedMult = 1.0f;
	reloadSpeedMultiplier.clear();
	slowdownOnHit = true;
	isDead = false;

	oldSpreadAngle = 0.0f;
	gunPos = oldMoveAmount = oldPos = pos = Vec2::zero();
	aimAngle = 0.0f;
	aimDir = Vec2::fromAngle(aimAngle);

	slowdownAmount = 0.0f;

	crosshairPos = Vec2(0.0f, 50.0f);
	crosshairDistance = 0.0f;
	moveSpeed = 0.0f;
	moveAngle = 0.0f;
	maxHitPoints = hitPoints = 100;
	data = lua.create_table();
	lua["player"] = this;
	updateHitPoints();
	secondaryGun = gun = nullptr;

	experience = 0;
	level = 1;
	updateExperience();
	experienceForNextLevel = calculateExperienceForLevel(level + 1);
	shootRenderable->playAnimation(0);

	joystickCheckTimer = 0.0f;

	setVisible(false);
}

float Player::getMonsterExperienceMultiplier() const
{
	return monsterExperienceMult;
}

float Player::getDamageMultiplier() const
{
	return damageMult;
}


void Player::resize()
{
	Mat3 barMat = Mat3::scaleMatrix(barSize / bloodworks->getCameraZoom()).translateBy(Vec2(0.0f, healthPos / bloodworks->getCameraZoom()));
	scaledBarSize = barSize / bloodworks->getCameraZoom();
	healthBarBG->setWorldMatrix(barMat);
	healthBarFG->setWorldMatrix(barMat);

	Vec2 expBarSize = Vec2(238, 11) * 0.3f;
	barMat = Mat3::scaleMatrix(expBarSize / bloodworks->getCameraZoom()).translateBy(Vec2(expPosX / bloodworks->getCameraZoom(), expPos / bloodworks->getCameraZoom()));
	scaledExpBarSize = expBarSize / bloodworks->getCameraZoom();
	experienceBarBG->setWorldMatrix(barMat);
	experienceBarFG->setWorldMatrix(barMat);

	updateHitPoints();
	updateExperience();
}

float Player::getReloadSpeedMultiplier()
{
	return reloadSpeedMultiplier.getBuffedValue();
}

float Player::getGlobalMonsterSpeedMultiplier()
{
	return globalMonsterSpeedMultiplier.getBuffedValue();
}

void Player::checkInput(bool& moving, float& wantedAngle)
{
	wantedAngle = moveAngle;

	if (input.hasJoyStick())
	{
		const Vec2& axis = input.getJoystickAxisPos();
		if (axis.lengthSquared() > 0.001f)
		{
			wantedAngle = axis.toAngle();
			moving = true;
		}

		/*if (input.isKeyDown(joystick_0_button_dpad_left) && input.isKeyDown(joystick_0_button_dpad_up))
		{
			wantedAngle = (pi + pi_d2) * 0.5f;
			moving = true;
		}
		else if (input.isKeyDown(joystick_0_button_dpad_left) && input.isKeyDown(joystick_0_button_dpad_down))
		{
			wantedAngle = -(pi + pi_d2) * 0.5f;
			moving = true;
		}
		else if (input.isKeyDown(joystick_0_button_dpad_right) && input.isKeyDown(joystick_0_button_dpad_up))
		{
			wantedAngle = pi_d2 * 0.5f;
			moving = true;
		}
		else if (input.isKeyDown(joystick_0_button_dpad_right) && input.isKeyDown(joystick_0_button_dpad_down))
		{
			wantedAngle = -pi_d2 * 0.5f;
			moving = true;
		}
		else if (input.isKeyDown(joystick_0_button_dpad_left))
		{
			wantedAngle = -pi;
			moving = true;
		}
		else if (input.isKeyDown(joystick_0_button_dpad_right))
		{
			wantedAngle = 0.0f;
			moving = true;
		}
		else if (input.isKeyDown(joystick_0_button_dpad_up))
		{
			wantedAngle = pi_d2;
			moving = true;
		}
		else if (input.isKeyDown(joystick_0_button_dpad_down))
		{
			wantedAngle = -pi_d2;
			moving = true;
		}
		*/
	}

	if (input.isKeyDown(key_a) && input.isKeyDown(key_w))
	{
		wantedAngle = (pi + pi_d2) * 0.5f;
		moving = true;
	}
	else if (input.isKeyDown(key_a) && input.isKeyDown(key_s))
	{
		wantedAngle = -(pi + pi_d2) * 0.5f;
		moving = true;
	}
	else if (input.isKeyDown(key_d) && input.isKeyDown(key_w))
	{
		wantedAngle = pi_d2 * 0.5f;
		moving = true;
	}
	else if (input.isKeyDown(key_d) && input.isKeyDown(key_s))
	{
		wantedAngle = -pi_d2 * 0.5f;
		moving = true;
	}
	else if (input.isKeyDown(key_a))
	{
		wantedAngle = -pi;
		moving = true;
	}
	else if (input.isKeyDown(key_d))
	{
		wantedAngle = 0.0f;
		moving = true;
	}
	else if (input.isKeyDown(key_w))
	{
		wantedAngle = pi_d2;
		moving = true;
	}
	else if (input.isKeyDown(key_s))
	{
		wantedAngle = -pi_d2;
		moving = true;
	}

}

void Player::updateExperience()
{
	float scale = scaledExpBarSize.x * (experience / (float)experienceForNextLevel) - 2.0f / bloodworks->getCameraZoom();
	if (scale > 0.01f && bloodworks->isMissionLoaded())
	{
		experienceBarActive->setVisible(true);
		experienceBarActive->setWorldMatrix(Mat3::scaleMatrix(scale, scaledExpBarSize.y - 2.0f / bloodworks->getCameraZoom()).translateBy(expPosX / bloodworks->getCameraZoom(), expPos / bloodworks->getCameraZoom()));
	}
	else
	{
		experienceBarActive->setVisible(false);
	}
}

void Player::killSelf()
{
	isDead = true;
	setVisible(false);
	bloodworks->onPlayerDied();
}
