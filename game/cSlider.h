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

	Vec2 bgSize;
	Vec2 sliderSize;

	float edgeShift;

	void setSliderPos(float pos);
public:
	cSlider(cGame *game);
	void check(const Vec2& mousePos);
};