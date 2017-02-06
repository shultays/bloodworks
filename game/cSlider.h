#pragma once

#include "cButton.h"

class cSlider : public cRenderableGroup
{
	cButton *bgButton;
	cButton *sliderButton;

	float pressShift;
	bool pressed;
	float minPos;
	float maxPos;
public:
	cSlider(cGame *game);
	void check(const Vec2& mousePos);
};