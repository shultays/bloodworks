#include "Player.h"
#include "Bloodworks.h"
#include "Bullet.h"
#include "Gun.h"
#include "cFont.h"
#include "cRenderable.h"

#include <sstream>


int Player::calculateExperienceForLevel(int level)
{
	return level * 100 + level * level * 20;
}

Player::Player(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	
	moveSpeedMult = shootSpeedMult = bulletSpeedMult = 1.0f;
	slowdownOnHit = true;

	oldSpreadAngle = 0.0f;
	oldMoveAmount = oldPos = pos = Vec2::zero();
	renderable = new cRenderableGroup(bloodworks);
	aimAngle = 0.0f;
	aimDir = Vec2::fromAngle(aimAngle);

	Mat3 mat = Mat3::identity();
	mat.scaleBy(15.7f, 22.9f);
	mat.translateBy(0.0f, 5.0f);


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
	crosshair->setWorldMatrix(Mat3::scaleMatrix(20.0f));
	bloodworks->addRenderable(crosshair, GUI + 100);

	spread = new cTexturedQuadRenderable(bloodworks, "resources/crosshair_spread.png", "resources/default");
	spread->setWorldMatrix(Mat3::scaleMatrix(20.0f));
	bloodworks->addRenderable(spread, GUI + 101);

	barSize = Vec2(256.0f, 32.0f);
	barSize *= 0.4f;
	barSize.x = round(barSize.x);
	barSize.y = round(barSize.y);
	Mat3 barMat = Mat3::scaleMatrix(barSize).translateBy(Vec2(0.0f, -50.0f));

	healthBarBG = new cTexturedQuadRenderable(bloodworks, "resources/assault/bar_bg.png", "resources/default");
	healthBarBG->setWorldMatrix(barMat);
	healthBarBG->setAlignment(RenderableAlignment::top);
	bloodworks->addRenderable(healthBarBG, GUI + 10);

	healthBarActive = new cTexturedQuadRenderable(bloodworks, "resources/assault/bar_active.png", "resources/default");
	healthBarActive->setWorldMatrix(barMat);
	healthBarActive->setAlignment(RenderableAlignment::top);
	healthBarActive->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.6f));
	bloodworks->addRenderable(healthBarActive, GUI + 11);

	healthBarFG = new cTexturedQuadRenderable(bloodworks, "resources/assault/bar_fg.png", "resources/default");
	healthBarFG->setWorldMatrix(barMat);
	healthBarFG->setAlignment(RenderableAlignment::top);
	bloodworks->addRenderable(healthBarFG, GUI + 12);

	slowdownAmount = 0.0f;

	crosshairPos = Vec2(10.0f);

	moveSpeed = 0.0f;
	moveAngle = 0.0f;
	maxHitPoints = hitPoints = 100;
	data = lua.create_table();
	lua["player"] = this;
	updateHitPoints();
	secondaryGun = gun = nullptr;

	experience = 0;
	level = 1;
	experienceForNextLevel = calculateExperienceForLevel(level + 1);
}

Player::~Player()
{
	SAFE_DELETE(crosshair);
	SAFE_DELETE(renderable);
	SAFE_DELETE(spread);
	SAFE_DELETE(healthRenderable);
	SAFE_DELETE(healthBarBG);
	SAFE_DELETE(healthBarActive);
	SAFE_DELETE(healthBarFG);
}

void Player::tick()
{
	oldPos = pos;

	float wantedAngle = moveAngle;

	bool moving = false;

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

	const float acceleration = 800.0f;
	const float decceleration = 1000.0f;
	float maxSpeed = 150.0f;
	float dt = timer.getDt();

	if (slowdownAmount > 0.0f && input.isMouseVisible() == false)
	{
		maxSpeed *= 1.0f - slowdownAmount;
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
			float rotation = lerp(maxRotation, minRotation, moveSpeed / maxSpeed) * dt;
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
		moveSpeed = min(moveSpeed, maxSpeed);
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
	crosshairPos += input.getDeltaMousePos() * bloodworks->getPauseSlowdown();

	float maxCrosshairDistance = gun->getMaxCrosshairDistance();
	float lengthSquared = crosshairPos.lengthSquared();
	if (lengthSquared > maxCrosshairDistance * maxCrosshairDistance)
	{
		crosshairPos = crosshairPos.normalized() * maxCrosshairDistance;
	}

	float length = 0.0f;
	if (crosshairPos.lengthSquared() > 0.01f)
	{
		aimDir = crosshairPos;
		length = aimDir.normalize();
		aimAngle = aimDir.toAngle();
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

		spread->setWorldMatrix(Mat3::scaleMatrix(sin(oldSpreadAngle) * length + 10.0f).translateBy(pos + crosshairPos));
		spread->setColor(Vec4(1.0f, 1.0f, 1.0f, clamped(oldSpreadAngle * 4, 0.0f, 0.4f)));

		spread->setVisible(true);
	}
	else
	{
		spread->setVisible(false);
	}

	crosshair->setWorldMatrix(Mat3::scaleMatrix(14.0f).translateBy(pos + crosshairPos));
	crosshair->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.7f));
	
	healthRenderable->setWorldMatrix(Mat3::translationMatrix(pos + Vec2(0.0f, 30.0f)));

	Mat3 mat = Mat3::identity();
	mat.rotateBy(pi_d2 - aimAngle);
	mat.translateBy(pos);
	renderable->setWorldMatrix(mat);

	if (gun)
	{
		gun->setTriggered(input.isKeyDown(mouse_button_left));
		gun->tick(dt);
	}

	if (secondaryGun)
	{
		secondaryGun->setTriggered(input.isKeyDown(mouse_button_right));
		secondaryGun->tick(dt);
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
			hitPoints = maxHitPoints;
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
	healthRenderable->setText(ss.str().c_str());
	float scale = (barSize.x - 9.0f) * (hitPoints / 100.0f);
	if (hitPoints > 1)
	{
		healthBarActive->setVisible(true);
		healthBarActive->setWorldMatrix(Mat3::scaleMatrix(Vec2(scale, barSize.y - 5.0f)).translateBy(Vec2(0.0f, -50.0f)));
	}
	else
	{
		healthBarActive->setVisible(false);
	}
}

void Player::gainExperience(int e)
{
	this->experience += e;
	if (experience >= experienceForNextLevel)
	{
		experience -= experienceForNextLevel;
		doLevelup();
	}
}

void Player::doLevelup()
{
	level++;
	experienceForNextLevel = calculateExperienceForLevel(level + 1);
	bloodworks->openLevelupPopup();
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
