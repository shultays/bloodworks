#include "Player.h"
#include "Bloodworks.h"
#include "Bullet.h"
#include "Gun.h"
#include "cFont.h"
#include "cRenderable.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "cTextRenderable.h"
#include "cAnimatedRenderable.h"
#include "DirentHelper.h"
#include "BloodworksControls.h"
#include "BloodRenderable.h"
#include "BloodworksConfig.h"
#include "CollisionController.h"
#include "cTexture.h"

#include <sstream>

const float healthPos = -50.0f;
const float expPos = -62.0f;
const float expPosX = -0.5f;

const float iconSize = 22.0f;

int Player::calculateExperienceForLevel(int level)
{
	return (level - 1) * 250 + (level - 1)  * (level - 1) * 20 + 100;
}

Player::Player(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	Mat3 mat = Mat3::identity();
	mat.scaleBy(15.7f, 22.9f);
	mat.translateBy(0.0f, 14.0f);

	renderable = new cRenderableContainer(bloodworks);
	cTexturedQuadRenderable *body = new cTexturedQuadRenderable(bloodworks, "resources/assault/body.png", "resources/default");
	body->setWorldMatrix(mat);
	renderable->addRenderable(body);

	//cTexturedQuadRenderable *hands = new cTexturedQuadRenderable(bloodworks, "resources/assault/hands.png", "resources/default");
	//hands->setWorldMatrix(mat);
	//renderable->addRenderable(hands);

	healthRenderable = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), "", 10);
	healthRenderable->setTextAlignment(TextAlignment::center);
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

	primaryRenderable = new cRenderableContainer(bloodworks);
	primaryRenderable->setWorldMatrix(Mat3::translationMatrix(50.0f, 50.0f));
	primaryRenderable->setAlignment(RenderableAlignment::bottomLeft);
	bloodworks->addRenderable(primaryRenderable, GUI + 103);

	primaryIcon = new cTexturedQuadRenderable(bloodworks, "", "resources/default");
	primaryIcon->setWorldMatrix(Mat3::scaleMatrix(40.0f));
	primaryRenderable->addRenderable(primaryIcon);

	primaryAmmo = new cTexturedQuadRenderable(bloodworks, "resources/reload_ring.png", "resources/reload");
	primaryAmmo->setWorldMatrix(Mat3::scaleMatrix(40.0f));
	primaryRenderable->addRenderable(primaryAmmo);

	secondaryRenderable = new cRenderableContainer(bloodworks);
	secondaryRenderable->setWorldMatrix(Mat3::translationMatrix(-50.0f, 50.0f));
	secondaryRenderable->setAlignment(RenderableAlignment::bottomRight);
	bloodworks->addRenderable(secondaryRenderable, GUI + 103);

	secondaryIcon = new cTexturedQuadRenderable(bloodworks, "", "resources/default");
	secondaryIcon->setWorldMatrix(Mat3::scaleMatrix(40.0f));
	secondaryRenderable->addRenderable(secondaryIcon);

	secondaryAmmo = new cTexturedQuadRenderable(bloodworks, "resources/reload_ring.png", "resources/reload");
	secondaryAmmo->setWorldMatrix(Mat3::scaleMatrix(40.0f));
	secondaryRenderable->addRenderable(secondaryAmmo);

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

	scoreText = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Score : 0", 22.0f);
	scoreText->setAlignment(RenderableAlignment::topRight);
	scoreText->setTextAlignment(TextAlignment::right);
	scoreText->setWorldMatrix(Mat3::translationMatrix(-20, -50));
	bloodworks->addRenderable(scoreText, GUI + 13);

	shootRenderable = new cAnimatedTexturedQuadRenderable(bloodworks, "resources/default");
	shootRenderable->addAnimation(cAnimatedTexturedQuadRenderable::AnimationData());
	
	shootRenderable->addAnimation(getAnimationData(DirentHelper::File("resources/assault/gun_fire/", "data.json")));
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

	primaryAmmo->addUniformFloat("uCurrentAmmo", 0.0f);
	primaryAmmo->addUniformInt("uMaxAmmo", 0);
	primaryAmmo->addUniformFloat("uReloading", 0.0f);

	secondaryAmmo->addUniformFloat("uCurrentAmmo", 0.0f);
	secondaryAmmo->addUniformInt("uMaxAmmo", 0);
	secondaryAmmo->addUniformFloat("uReloading", 0.0f);

	reset();
}

Player::~Player()
{
	SAFE_DELETE(crosshair);
	SAFE_DELETE(ammo);
	SAFE_DELETE(renderable);
	SAFE_DELETE(spread);
	SAFE_DELETE(healthRenderable);
	SAFE_DELETE(primaryRenderable);
	SAFE_DELETE(secondaryRenderable);

	SAFE_DELETE(healthBarBG);
	SAFE_DELETE(healthBarActive);
	SAFE_DELETE(healthBarFG);

	SAFE_DELETE(experienceBarBG);
	SAFE_DELETE(experienceBarActive);
	SAFE_DELETE(experienceBarFG);

	SAFE_DELETE(scoreText);

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
	maxSpeed.tick();
	maxRotateSpeed.tick();
	damageMultiplier.tick();
	shootSpeedMultiplier.tick();
	bulletSpeedMultiplier.tick();
	reloadSpeedMultiplier.tick();
	globalMonsterSpeedMultiplier.tick();
	clipCountMultiplier.tick();
	monsterExperienceMultiplier.tick();
	gunSpreadMultiplier.tick();
	accelerationMultiplier.tick();

	bool colorChanged = colorMultiplier.tick();
	if (colorChanged && renderable)
	{
		renderable->setColor(colorMultiplier.getBuffedValue());
	}

	oldPos = pos;

	float wantedAngle = moveAngle;

	bool moving = false;
	checkInput(moving, wantedAngle);
	const float acceleration = accelerationMultiplier.getBuffedValueFor(800.0f);
	const float decceleration = accelerationMultiplier.getBuffedValueFor(1000.0f);

	float dt = timer.getDt();
	float currentMaxSpeed = maxSpeed.getBuffedValue();

	float minRotation = pi * 2.0f;
	float maxRotation = pi * 10.0f;

	if (moving)
	{
		if (wantedAngle != moveAngle && currentMaxSpeed > 0.0001f)
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
	moveVelocity = moveDir * moveSpeed;

	Vec2 moveAmount = moveVelocity * dt;
	Vec2 newPos = pos + moveAmount;
	oldMoveAmount = moveAmount;
	AARect boundaries = bloodworks->getMapLimits();
	boundaries.addThreshold(-20.0f);

	float boundaryAmount = 40.0f;
	for (int i = 0; i < knockbacks.size(); i++)
	{
		Knockback& k = knockbacks[i];
		bool remove = false;
		float pushAmount = dt;
		if (k.duration < -0.5f)
		{
			pushAmount = dt;
		}
		else if (k.duration < dt)
		{
			pushAmount = k.duration;
		}
		k.duration -= dt;
		moveAmount += k.speed * pushAmount;

		if (k.duration <= 0.0f)
		{
			knockbacks.swapToTailRemove(i);
			i--;
		}
	}
	if (newPos.x < boundaries.getMin().x && moveAmount.x < 0.0f)
	{
		moveAmount.x = moveAmount.x * max(0.0f, (newPos.x + boundaryAmount - boundaries.getMin().x) / boundaryAmount);
	}
	else if (newPos.x > boundaries.getMax().x && moveAmount.x > 0.0f)
	{
		moveAmount.x = moveAmount.x * max(0.0f, (boundaries.getMax().x + boundaryAmount - newPos.x) / boundaryAmount);
	}

	if (newPos.y < boundaries.getMin().y && moveAmount.y < 0.0f)
	{
		moveAmount.y = moveAmount.y * max(0.0f, (newPos.y + boundaryAmount - boundaries.getMin().y) / boundaryAmount);
	}
	else if (newPos.y > boundaries.getMax().y && moveAmount.y > 0.0f)
	{
		moveAmount.y = moveAmount.y * max(0.0f, (boundaries.getMax().y + boundaryAmount - newPos.y) / boundaryAmount);
	}

	pos += moveAmount;
	Vec2 afterPos = pos;

	float collisionRadius = getCollisionRadius();

	bool solved = false;
	for(int i=0; i<5; i++)
	{
		Vec2 solver = bloodworks->getCollisionController()->getLongestSolver(Circle(pos, collisionRadius), CollisionController::no_player_collision);
		if (solver.lengthSquared() < 0.0001f)
		{
			solved = true;
			break;
		}
		else
		{
			pos += solver;
		}
	}

	if (solved == false)
	{
		for (int i = 1; i < 10; i++)
		{
			pos = (oldPos - afterPos) * ((float)i) / 9.0f + afterPos;
			if (i == 9)
			{
				pos = oldPos;
			}
			Vec2 solver = bloodworks->getCollisionController()->getLongestSolver(Circle(pos, collisionRadius), CollisionController::no_player_collision);
			if (solver.lengthSquared() == 0.0f)
			{
				break;
			}
		}
	}

	float sensitivityMult = bloodworks->getConfig()->getSensitivity();
	sensitivityMult = 0.5f * sensitivityMult * sensitivityMult + 0.5f;

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
			crosshairPos += input.getDeltaMousePos() * bloodworks->getPauseSlowdown() * sensitivityMult;
		}

		if (bloodworks->getConfig()->getLockCrosshair())
		{
			float maxCrosshairDistance = gun ? gun->getMaxCrosshairDistance() : 400.0f;
			float lengthSquared = crosshairPos.lengthSquared();
			if (lengthSquared > maxCrosshairDistance * maxCrosshairDistance)
			{
				crosshairPos = crosshairPos.normalized() * maxCrosshairDistance;
			}
		}
		else
		{
			Vec2 shift = bloodworks->getCameraPos() - pos;
			Vec2 screenMin = shift - bloodworks->getScreenDimensions().toVec() * 0.5f * bloodworks->getCameraZoom() + 40.0f;
			Vec2 screenMax = shift + bloodworks->getScreenDimensions().toVec() * 0.5f * bloodworks->getCameraZoom() - 40.0f;
			clamp(crosshairPos.x, screenMin.x, screenMax.x);
			clamp(crosshairPos.y, screenMin.y, screenMax.y);
		}
	}
	
	crosshairDistance = 0.0f;

	if (crosshairPos.lengthSquared() > 0.01f)
	{
		float newAngle = crosshairPos.toAngle();
		aimAngle = approachAngle(aimAngle, newAngle, maxRotateSpeed.getBuffedValue() * timer.getDt());
		aimDir = Vec2::fromAngle(aimAngle);
		crosshairDistance = crosshairPos.length();
	}
	if (gun && gun->getMaxAmmo() > 0)
	{
		if (gun->isReloading())
		{
			ammo->setUniform(currentAmmoUniformIndex, gun->getMaxAmmo() * gun->getReloadPercentage());
			primaryAmmo->setUniform(currentAmmoUniformIndex, gun->getMaxAmmo() * gun->getReloadPercentage());
		}
		else
		{
			ammo->setUniform(currentAmmoUniformIndex, (float)gun->getCurrentAmmo());
			primaryAmmo->setUniform(currentAmmoUniformIndex, (float)gun->getCurrentAmmo());
		}
		ammo->setUniform(maxAmmoUniformIndex, gun->getMaxAmmo());
		primaryAmmo->setUniform(maxAmmoUniformIndex, gun->getMaxAmmo());

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
		primaryAmmo->setUniform(reloadingUniformIndex, reloadAlpha);
		ammo->setVisible(true);
		primaryAmmo->setVisible(true);
		primaryAmmo->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	else
	{
		ammo->setVisible(false);
		primaryAmmo->setVisible(false);
	}

	if (secondaryGun)
	{
		if (secondaryGun->isReloading())
		{
			secondaryAmmo->setUniform(currentAmmoUniformIndex, secondaryGun->getMaxAmmo() * (1.0f - secondaryGun->getReloadPercentage()));
		}
		else
		{
			secondaryAmmo->setUniform(currentAmmoUniformIndex, (float)secondaryGun->getCurrentAmmo());
		}
		secondaryAmmo->setUniform(maxAmmoUniformIndex, secondaryGun->getMaxAmmo());

		if (secondaryGun->isReloading())
		{
			secondaryReloadAlpha = 1.0f;
		}
		else
		{
			secondaryReloadAlpha -= dt * 6.0f;
			if (secondaryReloadAlpha < 0.0f)
			{
				secondaryReloadAlpha = 0.0f;
			}
		}

		secondaryAmmo->setUniform(reloadingUniformIndex, secondaryReloadAlpha);
		secondaryAmmo->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	if (gun && gun->spreadVisible())
	{
		float newSpreadAngle = gunSpreadMultiplier.getBuffedValueFor(gun->getSpreadAngle());
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

	healthRenderable->setVisible(input.isKeyDown(key_space));
	gunPos = pos + aimDir * 24.0f - aimDir.sideVec() * 6.0f;

	Mat3 mat = Mat3::identity();
	mat.rotateBy(pi_d2 - aimAngle);
	mat.translateBy(pos);
	renderable->setWorldMatrix(mat);

	if (gun)
	{
		bool trigerred = bloodworks->getPauseSlowdown() > 0.5f && mapper.isKeyDown(GameKey::Attack);
		gun->setTriggered(trigerred);
		gun->tick(dt);
		gun->updateLaser(gunPos, -aimAngle);
	}

	if (secondaryGun)
	{
		bool trigerred = bloodworks->getPauseSlowdown() > 0.5f && mapper.isKeyDown(GameKey::Attack2);
		secondaryGun->setTriggered(trigerred);
		secondaryGun->tick(dt);
		if (secondaryGun)
		{
			secondaryGun->updateLaser(gunPos, -aimAngle);
		}
	}
	for (int i = 0; i < oldGuns.size(); i++)
	{
		oldGuns[i].timeToTick -= dt;
		if (oldGuns[i].timeToTick < 0.0f)
		{
			oldGuns.swapToTailRemove(i);
			i--;
		}
		else
		{
			oldGuns[i].gun->setTriggered(false);
			oldGuns[i].gun->tick(dt);
			oldGuns[i].gun->updateLaser(gunPos, -aimAngle);
		}
	}
}

void Player::setGun(Gun *gun)
{
	if (this->gun)
	{
		this->gun->stop();
		auto& oldGun = oldGuns.insertAndGetReference();
		oldGun.gun = this->gun;
		oldGun.timeToTick = 30.0f;
	}

	this->gun = gun;
	for (int i = 0; i < oldGuns.size(); i++)
	{
		if (oldGuns[i].gun == gun)
		{
			oldGuns.swapToTailRemove(i);
			break;
		}
	}
	if (this->gun)
	{
		shootRenderable->setColor(this->gun->getShootingParticleColor());
		this->gun->start();
		primaryIcon->setTexture(this->gun->getIconPath());
		Vec2 textureSize = primaryIcon->getTexture()->getDimensions().toVec();
		if (textureSize.w > iconSize)
		{
			textureSize *= iconSize / textureSize.w;
		}
		if (textureSize.h > iconSize * 0.7f)
		{
			textureSize *= iconSize * 0.7f / textureSize.h;
		}
		primaryIcon->setWorldMatrix(Mat3::scaleMatrix(textureSize).rotateBy(-pi * 0.15f));
	}
	//primaryRenderable->setVisible( this->gun != nullptr);
}

void Player::setSecondaryGun(Gun *gun)
{
	if (this->secondaryGun)
	{
		this->secondaryGun->stop();
		auto& oldGun = oldGuns.insertAndGetReference();
		oldGun.gun = this->secondaryGun;
		oldGun.timeToTick = 30.0f;
	}

	this->secondaryGun = gun;
	for (int i = 0; i < oldGuns.size(); i++)
	{
		if (oldGuns[i].gun == gun)
		{
			oldGuns.swapToTailRemove(i);
			break;
		}
	}
	if (this->secondaryGun)
	{
		this->secondaryGun->start();
		secondaryIcon->setTexture(this->secondaryGun->getIconPath());
		Vec2 textureSize = secondaryIcon->getTexture()->getDimensions().toVec();
		if (textureSize.w > iconSize)
		{
			textureSize *= iconSize / textureSize.w;
		}
		if (textureSize.h > iconSize * 0.7f)
		{
			textureSize *= iconSize * 0.7f / textureSize.h;
		}
		secondaryIcon->setWorldMatrix(Mat3::scaleMatrix(textureSize).rotateBy(-pi * 0.15f));
	}
	secondaryRenderable->setVisible(this->secondaryGun != nullptr);
}

Gun* Player::getGun()
{
	return gun;
}

Gun* Player::getSecondaryGun()
{
	return secondaryGun;
}

void Player::addScore(int score)
{
	setScore(this->score + score);
}

void Player::setScore(int score)
{
	this->score = score;
	std::stringstream ss;
	ss << "Score : " << this->score;
	scoreText->setText(ss.str());
}

int Player::doDamage(int damage, float angle)
{
	return doDamageWithArgs(damage, angle, lua.create_table());
}

int Player::doDamageWithArgs(int damage, float angle, sol::table& params)
{
	damage = bloodworks->onPlayerDamaged(damage, angle, params);
	if (damage > 0)
	{
		Vec2 dir = Vec2::fromAngle(angle);
		bloodworks->getBloodRenderable()->addBlood(pos, dir * clamped(damage * 2.7f, 0.0f, 20.0f), 10.0f + randFloat(10.0f));

		hitPoints -= damage;
		if (hitPoints <= 0)
		{
			hitPoints = 0;
			killSelf();
			killSounds[randInt(killSounds.size())]->play();
		}
		else
		{
			hitSounds[randInt(hitSounds.size())]->play();
		}
		updateHitPoints();
	}
	return damage;
}

void Player::updateHitPoints()
{
	std::stringstream ss;
	ss << hitPoints;
	healthRenderable->setText(ss.str());
	float scale = (scaledBarSize.x - 9.0f) * (hitPoints / (float)maxHitPoints);
	if ((isDead == false && hitPoints > 1) && bloodworks->isMissionLoaded())
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
	if (experience >= experienceForNextLevel && bloodworks->isLevelUpPopupVisible() == false && isDead == false)
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
	bloodworks->onLevelUp();
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
	scoreText->setVisible(visible);

	healthBarBG->setVisible(visible);
	healthBarActive->setVisible(visible);
	healthBarFG->setVisible(visible);

	experienceBarBG->setVisible(visible);
	experienceBarActive->setVisible(visible);
	experienceBarFG->setVisible(visible);
	primaryRenderable->setVisible(false && gun != nullptr);
	secondaryRenderable->setVisible(visible && secondaryGun != nullptr);
	updateHitPoints();
	updateExperience();
}

void Player::reset()
{
	knockbacks.clear();
	maxSpeed.clear();
	monsterExperienceMultiplier.clear();
	damageMultiplier.clear();
	shootSpeedMultiplier.clear();
	reloadSpeedMultiplier.clear();
	bulletSpeedMultiplier.clear();
	globalMonsterSpeedMultiplier.clear();
	clipCountMultiplier.clear();
	gunSpreadMultiplier.clear();
	accelerationMultiplier.clear();

	maxSpeed.setBaseValue(150.0f);
	maxRotateSpeed.setBaseValue(pi * 6.0f);

	isDead = false;

	reloadAlpha = 0.0f;
	secondaryReloadAlpha = 0.0f;

	oldSpreadAngle = 0.0f;
	gunPos = oldMoveAmount = oldPos = pos = Vec2::zero();
	aimAngle = pi_d2;
	aimDir = Vec2::fromAngle(aimAngle);

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
	setScore(0);
	setVisible(false);
}

float Player::getMonsterExperienceMultiplier()
{
	return monsterExperienceMultiplier.getBuffedValue();
}

float Player::getDamageMultiplier() 
{
	return damageMultiplier.getBuffedValue();
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

	float shift = 80.0f / bloodworks->getCameraZoom();

	primaryRenderable->setWorldMatrix(Mat3::scaleMatrix( 1 / bloodworks->getCameraZoom()).translateBy(shift, shift));
	secondaryRenderable->setWorldMatrix(Mat3::scaleMatrix( 1 / bloodworks->getCameraZoom()).translateBy(-shift, shift));

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

float Player::getClipCountMultiplier()
{
	return clipCountMultiplier.getBuffedValue();
}

int Player::getBuffedClipSize(int clipSize)
{
	return (int)ceil(clipCountMultiplier.getBuffedValueFor((float)clipSize) - 0.01f);
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
	}

	if (mapper.isKeyDown(GameKey::Left) && mapper.isKeyDown(GameKey::Up))
	{
		wantedAngle = (pi + pi_d2) * 0.5f;
		moving = true;
	}
	else if (mapper.isKeyDown(GameKey::Left) && mapper.isKeyDown(GameKey::Down))
	{
		wantedAngle = -(pi + pi_d2) * 0.5f;
		moving = true;
	}
	else if (mapper.isKeyDown(GameKey::Right) && mapper.isKeyDown(GameKey::Up))
	{
		wantedAngle = pi_d2 * 0.5f;
		moving = true;
	}
	else if (mapper.isKeyDown(GameKey::Right) && mapper.isKeyDown(GameKey::Down))
	{
		wantedAngle = -pi_d2 * 0.5f;
		moving = true;
	}
	else if (mapper.isKeyDown(GameKey::Left))
	{
		wantedAngle = -pi;
		moving = true;
	}
	else if (mapper.isKeyDown(GameKey::Right))
	{
		wantedAngle = 0.0f;
		moving = true;
	}
	else if (mapper.isKeyDown(GameKey::Up))
	{
		wantedAngle = pi_d2;
		moving = true;
	}
	else if (mapper.isKeyDown(GameKey::Down))
	{
		wantedAngle = -pi_d2;
		moving = true;
	}
}

void Player::updateExperience()
{
	float scale = scaledExpBarSize.x * (experience / (float)experienceForNextLevel) - 2.0f / bloodworks->getCameraZoom();
	scale = min(scale, scaledExpBarSize.x);
	if (scale > 0.01f && bloodworks->isMissionLoaded())
	{
		experienceBarActive->setVisible(visible);
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
	setGun(nullptr);
	setSecondaryGun(nullptr);
	bloodworks->onPlayerDied();
}

float Player::getBulletRadius() const
{
	return 10.0f;
}

float Player::getCollisionRadius() const
{
	return 15.0f;
}

void Player::addKnockback(const Vec2& speed, float duration)
{
	Knockback& k = knockbacks.insertAndGetReference();
	k.speed = speed;
	k.duration = duration;
}