#include "cButton.h"

void cButton::check(const Vec2& mousePos)
{
	prevDown = down;
	prevHovering = hovering;

	hovering = down = false;

	Vec2 diff = mousePos - defaultShift;

	if (getAlignment() == RenderableAlignment::topLeft)
	{
		diff -= Vec2(0.0f, (float)game->getScreenDimensions().h);
	}

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
