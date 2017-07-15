#include "cButton.h"
#include "cSound.h"
#include "cGame.h"
#include "cGlobals.h"

void cButton::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	cRenderableContainer::render(isIdentity, mat, crop);
	lastRenderCrop = crop;
}

void cButton::check(const Vec2& mousePos, bool ignoreClick)
{
	prevDown = down;
	prevHovering = hovering;

	hovering = down = false;

	Vec2 transformedMousePos = game->getRelativeMousePos(mousePos, getAlignment());
	Vec2 diff = transformedMousePos - defaultShift;

	if (enforceHovering == enforce_hovering 
		|| (enforceHovering != enforce_not_hovering && diff.x > beginRange.x && diff.x < endRange.x && diff.y > beginRange.y && diff.y < endRange.y))
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
	setWorldMatrix(Mat3::scaleMatrix(lerp(defaultScale, hoverScale, hoverTime)).rotateBy(lerp(defaultRotation, hoverRotation, hoverTime)).translateBy(lerp(defaultShift, hoverShift, hoverTime)));
}
