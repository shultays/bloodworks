#include "cInput.h"
#include "cGlobals.h"
#include <stdio.h>
#include <string.h>

void cInput::init()
{
	memset(keyStates, 0, sizeof(keyStates));
	memset(prevKeyStates, 0, sizeof(prevKeyStates));
	memset(keyTime, 0, sizeof(keyTime));
	ignoreNextMove = true;
	hideMouse();
	prevMousePos = mousePos = Vec2::zero();
}

void cInput::pressKey(int key)
{
	keyStates[key] = 1;
	keyTime[key] = timer.getTime();
}

void cInput::tick()
{
	memcpy(prevKeyStates, keyStates, sizeof(prevKeyStates));
	prevMousePos = mousePos;
}

void cInput::releaseKey(int key)
{
	keyStates[key] = 0;
	keyTime[key] = timer.getTime();
}

void cInput::setMousePos(const Vec2& pos, const Vec2& relativePos)
{
	if (mouseShown)
	{
		mousePos = Vec2(pos.x, pos.y);
	}
	else
	{
		prevMousePos -= relativePos;
	}
	if (ignoreNextMove)
	{
		ignoreNextMove = false;
		prevMousePos = mousePos;
	}
}
