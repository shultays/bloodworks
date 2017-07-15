#pragma once

#include "cRenderable.h"

class cButton;

class cSlider : public cRenderableGroup
{
	cButton *bgButton;
	cButton *sliderButton;

	float pressShift;
	bool pressed;
	float minPos;
	float maxPos;

	Vec2 bgSize;
	Vec2 originalBgSize;
	Vec2 sliderSize;

	float edgeShift;

	void setSliderPos(float pos);

	int minValue;
	int maxValue;
	int curValue;

	float minValueF;
	float maxValueF;
	float curValueF;

	bool isFloat;
	bool valueChanged;

	bool isVertical;
public:
	cSlider(cGame *game, bool isVertical = false);
	void check(const Vec2& mousePos, bool ignoreClick = false); // todo this shouldnt called manually and handle transforms
	void setMinMax(float min, float max);
	void setMinMax(int min, int max);

	float getValue() const
	{
		return isFloat ? curValueF : (float)curValue;
	}

	int getIntValue() const
	{
		return isFloat ? (int)curValueF : curValue;
	}

	float getFloatValue() const
	{
		return getValue();
	}

	void setValue(int value);
	void setValue(float value);

	bool isChanged() const
	{
		return valueChanged;
	}

	void setLength(float length);
};