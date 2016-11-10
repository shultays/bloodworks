#include "Player.h"
#include "Bloodworks.h"
#include "Bullet.h"
#include "Gun.h"


Player::Player(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	lua.new_usertype<Player>("Player",
		"position", &Player::pos,
		"moveSpeed", &Player::moveSpeed,
		"moveAngle", &Player::moveAngle,
		"crosshairPos", &Player::crosshairPos,
		"aimDir", sol::readonly(&Player::aimDir)
		);

	oldSpreadAngle = 0.0f;
	pos = Vec2::zero();
	renderable = new cRenderableGroup(bloodworks);
	aimAngle = 0.0f;
	aimDir = Vec2::fromAngle(aimAngle);

	Mat3 mat = Mat3::identity();
	mat.scaleBy(15.7f, 22.9f);
	mat.translateBy(0.0f, 5.0f);


	cTexturedQuadRenderable *body = new cTexturedQuadRenderable(bloodworks, "resources/assault/body.png", "resources/default");
	renderable->addRenderable(body, mat);

	cTexturedQuadRenderable *hands = new cTexturedQuadRenderable(bloodworks, "resources/assault/hands.png", "resources/default");
	renderable->addRenderable(hands, mat);

	renderable->setWorldMatrix(Mat3::identity());
	bloodworks->addRenderable(renderable, 500);

	crosshair = new cTexturedQuadRenderable(bloodworks, "resources/crosshair.png", "resources/default");
	crosshair->setSize(20.0f);
	bloodworks->addRenderable(crosshair, 502);

	crosshairPos = Vec2(10.0f);

	moveSpeed = 0.0f;
	moveAngle = 0.0f;

	lua["player"] = this;

	this->gun = nullptr;
}

Player::~Player()
{
	SAFE_DELETE(crosshair);
	SAFE_DELETE(renderable);
}

void Player::tick(float dt)
{
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
	const float maxSpeed = 150.0f;

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

	pos += Vec2::fromAngle(moveAngle) * moveSpeed * dt;


	crosshairPos += input.getDeltaMousePos();

	float maxCrosshairDistance = gun->getMaxCrosshairDistance();
	float lengthSquared = crosshairPos.lengthSquared();
	if (lengthSquared > maxCrosshairDistance * maxCrosshairDistance)
	{
		crosshairPos = crosshairPos.normalized() * maxCrosshairDistance;
	}
	crosshair->setPosition(pos + crosshairPos);

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
		crosshair->setSize(sin(newSpreadAngle) * length + 10.0f);
		oldSpreadAngle = newSpreadAngle;
	}

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
