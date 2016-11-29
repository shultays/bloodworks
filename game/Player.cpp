#include "Player.h"
#include "Bloodworks.h"
#include "Bullet.h"
#include "Gun.h"
#include "cFont.h"
#include <sstream>


Player::Player(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	lua.new_usertype<Player>("Player",
		"position", &Player::pos,
		"moveSpeed", &Player::moveSpeed,
		"moveAngle", &Player::moveAngle,
		"crosshairPos", &Player::crosshairPos,
		"aimDir", sol::readonly(&Player::aimDir),
		"moveDir", sol::readonly(&Player::moveDir),
		"moveSpeedDir", sol::readonly(&Player::moveSpeedDir),
		"hitPoints", sol::readonly(&Player::hitPoints),
		"doDamage", &Player::doDamage,
		"slowdown", &Player::slowdown,
		"bulletSpeedMult", &Player::bulletSpeedMult,
		"shootSpeedMult", &Player::shootSpeedMult,
		"moveSpeedMult", &Player::moveSpeedMult
		);

	moveSpeedMult = shootSpeedMult = bulletSpeedMult = 1.0f;

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
	bloodworks->addRenderable(crosshair, FOREGROUND + 1);

	spread = new cTexturedQuadRenderable(bloodworks, "resources/crosshair_spread.png", "resources/default");
	spread->setWorldMatrix(Mat3::scaleMatrix(20.0f));
	bloodworks->addRenderable(spread, FOREGROUND + 2);

	slowdownAmount = 0.0f;

	crosshairPos = Vec2(10.0f);

	moveSpeed = 0.0f;
	moveAngle = 0.0f;
	hitPoints = 100;
	lua["player"] = this;
	updateHitPoints();
	gun = nullptr;
}

Player::~Player()
{
	SAFE_DELETE(crosshair);
	SAFE_DELETE(renderable);
	SAFE_DELETE(spread);
	SAFE_DELETE(healthRenderable);
}

void Player::tick(float dt)
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

	if (slowdownAmount > 0.0f)
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
	crosshairPos += input.getDeltaMousePos();

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

	if (gun)
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
	}

	crosshair->setWorldMatrix(Mat3::scaleMatrix(14.0f).translateBy(pos + crosshairPos));
	crosshair->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.7f));
	
	spread->setWorldMatrix(Mat3::scaleMatrix(sin(oldSpreadAngle) * length + 10.0f).translateBy(pos + crosshairPos));
	spread->setColor(Vec4(1.0f, 1.0f, 1.0f, clamped(oldSpreadAngle * 4, 0.0f, 0.4f)));

	healthRenderable->setWorldMatrix(Mat3::translationMatrix(pos + Vec2(0.0f, 30.0f)));


	Mat3 mat = Mat3::identity();
	mat.rotateBy(pi_d2 - aimAngle);
	mat.translateBy(pos);
	renderable->setWorldMatrix(mat);

	if (gun)
	{
		gun->tick(dt);
	}
}

void Player::setGun(Gun *gun)
{
	if (gun)
	{
		gun->stop();
	}

	this->gun = gun;

	if (gun)
	{
		gun->start();
	}
}

Gun* Player::getGun()
{
	return gun;
}

void Player::doDamage(int damage)
{
	hitPoints -= damage;
	if (hitPoints <= 0)
	{
		hitPoints = 100;
	}
	updateHitPoints();
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
}
