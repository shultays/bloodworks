#include "cButton.h"
#include "cSound.h"
#include "cGame.h"
#include "cGlobals.h"

void cButton::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	if (isDirty)
	{
		setWorldMatrix(Mat3::scaleMatrix(lerp(defaultScale, hoverScale, hoverTime)).rotateBy(lerp(defaultRotation, hoverRotation, hoverTime)).translateBy(lerp(defaultShift, hoverShift, hoverTime)));
	}
	cRenderableContainer::render(isIdentity, mat, crop);
	lastRenderCrop = crop;
}

void cButton::check(const Vec2& mousePos, bool ignoreClick)
{
	prevDown = down;
	prevHovering = hovering;

	hovering = down = false;

	if (visible == false)
	{
		return;
	}

	Vec2 transformedMousePos = game->getRelativeMousePos(mousePos, getAlignment());
	Vec2 diff = transformedMousePos - defaultShift;

	if (enforceHovering == enforce_hovering 
		|| (enforceHovering != enforce_not_hovering && hitRect.isInside(diff)))
	{
		hovering = true;
		down = hovering && ignoreClick == false && input.isKeyDown(mouse_button_left);
	}
	else
	{
		prevDown = false;
	}

	if (input.isKeyPressed(mouse_button_left) && ignoreClick == false)
	{
		pressedInside = hovering;
	}

	if (hovering)
	{
		hoverTime += timer.getNonSlowedDt() * hoverSpeed;
	}
	else
	{
		hoverTime -= timer.getNonSlowedDt() * hoverSpeed;
	}

	if (isClicked() && clickSound != nullptr)
	{
		clickSound->play();
	}
	if (isEntered() && hoverSound != nullptr)
	{
		hoverSound->play();
	}
	saturate(hoverTime);
	isDirty = false;
	setWorldMatrix(Mat3::scaleMatrix(lerp(defaultScale, hoverScale, hoverTime)).rotateBy(lerp(defaultRotation, hoverRotation, hoverTime)).translateBy(lerp(defaultShift, hoverShift, hoverTime)));
}
