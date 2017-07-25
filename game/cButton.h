#pragma once

#include "cRenderableContainer.h"
#include "cResources.h"

class cButton : public cRenderableContainer
{
	bool hovering;
	bool prevHovering;
	bool down;
	bool prevDown;

	Vec2 beginRange; // todo change rect
	Vec2 endRange;

	Vec2 hoverShift;
	Vec2 hoverScale;
	float hoverRotation;

	Vec2 defaultShift;
	Vec2 defaultScale;
	float defaultRotation;

	bool pressedInside;
	float hoverTime;
	float hoverSpeed;

	cSoundSampleShr clickSound;
	cSoundSampleShr hoverSound;

	int enforceHovering;

	Rect lastRenderCrop;
	void *userData;
	bool isDirty = false;
public:
	enum
	{
		no_enforce,
		enforce_hovering,
		enforce_not_hovering
	};
	cButton(cGame *game) : cRenderableContainer(game)
	{
		down = prevDown = false;
		hovering = prevHovering = false;

		beginRange = Vec2(-100.0f);
		endRange = Vec2(100.0f);

		pressedInside = false;

		defaultShift.setZero();
		hoverShift.setZero();
		defaultScale = 1.0f;
		hoverScale = 1.0f;
		defaultRotation = 0.0f;
		hoverRotation = 0.0f;
		hoverTime = 0.0f;
		hoverSpeed = 1.0f;

		enforceHovering = no_enforce;
		lastRenderCrop = Rect::invalid();
		userData = nullptr;
		isDirty = true;
	}

	void setUserData(void *userData)
	{
		this->userData = userData;
	}

	void* getUserData() const
	{
		return userData;
	}

	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;

	void setEnforcedHovering(int enforceHovering)
	{
		this->enforceHovering = enforceHovering;
	}
	
	void setHitArea(const Vec2& beginRange, const Vec2& endRange)
	{
		this->beginRange = beginRange;
		this->endRange = endRange;
	}

	void check(const Vec2& mousePos, bool ignoreClick = false);

	bool isDown() const
	{
		return down;
	}

	bool isHovering() const
	{
		return hovering;
	}

	bool isEntered() const
	{
		return hovering && !prevHovering;
	}

	bool isExited() const
	{
		return !hovering && prevHovering;
	}

	bool isPressed() const
	{
		return down && !prevDown;
	}

	bool isReleased() const
	{
		return !down && prevDown;
	}

	bool isClicked() const
	{
		return pressedInside && !down && prevDown;
	}

	void setHoverShift(const Vec2& shift)
	{
		this->hoverShift = shift;
	}

	void setHoverScale(const Vec2& scale)
	{
		this->hoverScale = scale;
	}

	void setHoverMatrix(const Vec2& shift, const Vec2& scale, float rotation)
	{
		this->hoverShift = shift;
		this->hoverScale = scale;
		this->hoverRotation = rotation;
	}

	void setHoverSpeed(float hoverSpeed)
	{
		this->hoverSpeed = hoverSpeed;
	}

	void setDefaultShift(const Vec2& shift)
	{
		this->defaultShift = shift;
	}

	void setDefaultScale(const Vec2& scale)
	{
		this->defaultScale = scale;
	}

	void setDefaultMatrix(const Vec2& shift, const Vec2& scale, float rotation)
	{
		this->defaultShift = shift;
		this->defaultScale = scale;
		this->defaultRotation = rotation;
		setWorldMatrix(Mat3::scaleMatrix(defaultScale).rotateBy(defaultRotation).translateBy(defaultShift));
	}

	void setSounds(cSoundSampleShr clickSound, cSoundSampleShr hoverSound)
	{
		this->clickSound = clickSound;
		this->hoverSound = hoverSound;
	}

	void setHoverWeight(float hoverWeight)
	{
		this->hoverTime = hoverWeight;
	}
};