#pragma once

#include "cRenderable.h"
#include "cTexture.h"
#include "cGame.h"

class cButton : public cTexturedQuadRenderable
{
	bool hovering;
	bool prevHovering;
	bool down;
	bool prevDown;

	Vec2 beginRange;
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
public:
	cButton(cGame *game, const std::string& texturePath, const std::string& shaderPath) : cTexturedQuadRenderable(game, texturePath, shaderPath)
	{
		down = prevDown = false;
		hovering = prevHovering = false;

		beginRange = -texture[0]->getDimensions().toVec() * 0.5f;
		endRange = texture[0]->getDimensions().toVec() * 0.5f;

		pressedInside = false;

		hoverShift.setZero();
		hoverScale = 100.0f;
		hoverRotation = 0.0f;
		hoverTime = 0.0f;
		hoverSpeed = 1.0f;
	}
	
	void setHitArea(const Vec2& beginRange, const Vec2& endRange)
	{
		this->beginRange = beginRange;
		this->endRange = endRange;
	}

	void check(const Vec2& mousePos)
	{
		prevDown = down;
		prevHovering = hovering;

		hovering = down = false;
		
		Vec2 diff = mousePos - defaultShift;

		if (diff.x > beginRange.x && diff.x < endRange.x && diff.y > beginRange.y && diff.y < endRange.y)
		{
			hovering = true;
			down = hovering && input.isKeyDown(mouse_button_left);
		}
		else
		{
			prevDown = false;
		}

		if (input.isKeyPressed(mouse_button_left))
		{
			pressedInside = hovering;
		}

		if (hovering)
		{
			hoverTime += timer.getDt() * hoverSpeed;
		}
		else
		{
			hoverTime -= timer.getDt() * hoverSpeed;
		}
		saturate(hoverTime);
		setWorldMatrix(Mat3::scaleMatrix(lerp(defaultScale, hoverScale, hoverTime)).rotateBy(lerp(defaultRotation, hoverRotation, hoverTime)).translateBy(lerp(defaultShift, hoverShift, hoverTime)));
	}

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

	void setDefaultMatrix(const Vec2& shift, const Vec2& scale, float rotation)
	{
		this->defaultShift = shift;
		this->defaultScale = scale;
		this->defaultRotation = rotation;
		setWorldMatrix(Mat3::scaleMatrix(defaultScale).rotateBy(defaultRotation).translateBy(defaultShift));
	}
};