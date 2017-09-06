#include "cSlider.h"
#include "cButton.h"
#include "cTexturedQuadRenderable.h"
#include "cTexture.h"
#include "cGame.h"
#include "cGlobals.h"

cSlider::cSlider(cGame *game, bool isVertical) : cRenderableContainer(game)
{
	bgButton = new cButton(game);
	cTexturedQuadRenderable *quad = new cTexturedQuadRenderable(game, isVertical ? "resources/ui/slider_bg_vertical.png":"resources/ui/slider_bg.png", "resources/default");
	bgSize = quad->getTexture()->getDimensions().toVec();
	originalBgSize = bgSize;
	quad->setWorldMatrix(Mat3::identity());
	bgButton->addRenderable(quad);
	addRenderable(bgButton);
	bgButton->setDefaultMatrix(Vec2(0.0f), bgSize, 0.0f);
	bgButton->setHoverMatrix(Vec2(0.0f), bgSize, 0.0f);
	bgButton->setHitArea(-bgSize, bgSize);

	sliderButton = new cButton(game);
	quad = new cTexturedQuadRenderable(game, "resources/ui/slider_fg.png", "resources/default");
	quad->setWorldMatrix(Mat3::identity());
	sliderSize = quad->getTexture()->getDimensions().toVec();
	sliderButton->addRenderable(quad);
	sliderButton->setDefaultMatrix(Vec2(0.0f), sliderSize, 0.0f);
	sliderButton->setHoverMatrix(Vec2(0.0f), sliderSize, 0.0f);
	sliderButton->setHitArea(-sliderSize, sliderSize);
	sliderButton->setHoverSpeed(10.0f);
	addRenderable(sliderButton);

	this->isVertical = isVertical;

	pressed = false;
	edgeShift = 0.0f;

	isFloat = true;
	minValue = 0;
	maxValue = 10;
	curValue = 5;

	minValueF = 0.0f;
	maxValueF = 1.0f;
	curValueF = 0.5f;
	setValue(0.5f);
	valueChanged = false;
}

void cSlider::check(const Vec2& mousePos, bool ignoreClick)
{
	valueChanged = false;

	Vec2 shiftedPos = mousePos - getPosition();
	shiftedPos /= game->getCameraZoom(); // todo fix
	int axis = isVertical ? 1 : 0;
	Vec2 relativeMouse = game->getRelativeMousePos(mousePos, getAlignment()) - getPosition();

	float maxShift = bgSize[axis] - sliderSize[axis] - edgeShift;

	if (pressed)
	{
		float pos = relativeMouse[axis] - pressShift;
		clamp(pos, -maxShift, maxShift);
		setSliderPos(pos);
	}

	Vec2 childPos = mousePos - getPosition() / game->getCameraZoom();
	bgButton->check(childPos, ignoreClick);
	sliderButton->check(childPos, ignoreClick);

	if (input.getMouseWheel() && bgButton->isHovering())
	{
		if (isFloat)
		{
			float shift = input.getMouseWheel() * 0.05f * (maxValueF - minValueF);
			setValue(getValue() + shift);
		}
		else
		{
			int shift = (int)round(input.getMouseWheel() * 0.05f * (maxValue - minValue));
			if (input.getMouseWheel() > 0 && shift <= 0)
			{
				shift = 1;
			}
			else if (input.getMouseWheel() < 0 && shift >= 0)
			{
				shift = -1;
			}
			setValue(getIntValue() + shift);
		}
		input.clearWheel();
	}

	if (sliderButton->isPressed())
	{
		pressShift = relativeMouse[axis] - sliderButton->getPosition()[axis];
		pressed = true;
	}

	if (bgButton->isReleased())
	{
		if (pressed == false)
		{
			float pos = relativeMouse[axis];
			clamp(pos, -maxShift, maxShift);
			setSliderPos(pos);
		}
	}
	if (input.isKeyDown(mouse_button_left) == false)
	{
		pressed = false;
	}
}

void cSlider::setMinMax(float min, float max)
{
	isFloat = true;
	this->minValueF = min;
	this->maxValueF = max;

	clamp(curValueF, min, max);
}


void cSlider::setMinMax(int min, int max)
{
	isFloat = false;
	this->minValue = min;
	this->maxValue = max;

	clamp(curValue, min, max);
}

void cSlider::setValue(int value)
{
	int axis = isVertical ? 1 : 0;
	if (isFloat)
	{
		setValue((float)value);
		return;
	}
	curValue = value;
	clamp(curValue, minValue, maxValue);
	float maxShift = bgSize[axis] - sliderSize[axis] - edgeShift;
	float t = ((float)curValue - minValue) / (maxValue - minValue);
	float pos = -maxShift + maxShift * 2.0f * t;
	setSliderPos(pos);
}

void cSlider::setValue(float value)
{
	int axis = isVertical ? 1 : 0;
	if (isFloat == false)
	{
		setValue((int)round(value));
		return;
	}

	curValueF = value;
	clamp(curValueF, minValueF, maxValueF);
	float maxShift = bgSize[axis] - sliderSize[axis] - edgeShift;
	float t = (curValueF - minValueF) / (maxValueF - minValueF);
	float pos = -maxShift + maxShift * 2.0f * t;
	setSliderPos(pos);

	valueChanged = false;
}

void cSlider::setLength(float length)
{
	int axis = isVertical ? 1 : 0;
	bgSize[axis] = length;

	bgButton->setDefaultMatrix(Vec2(0.0f), bgSize, 0.0f);
	bgButton->setHoverMatrix(Vec2(0.0f), bgSize, 0.0f);
	bgButton->setHitArea(-bgSize, bgSize);
	if (isFloat)
	{
		setValue(curValueF);
	}
	else
	{
		setValue(curValue);
	}
}

void cSlider::setSliderPos(float pos)
{
	int axis = isVertical ? 1 : 0;
	float maxShift = bgSize[axis] - sliderSize[axis] - edgeShift;
	float t = (pos + maxShift) / (2 * maxShift);
	if (isFloat == false)
	{
		int count = maxValue - minValue;
		int r = (int)round(t * count);
		curValue = r - minValue;
		t = ((float)r) / count;
		pos = -maxShift + maxShift * 2.0f * t;
	}
	else
	{
		curValueF = minValueF + (maxValueF - minValueF) * t;
		clamp(curValueF, minValueF, maxValueF);
	}
	Vec2 coor = isVertical ? Vec2(0.0f, pos) : Vec2(pos, 0.0f);
	sliderButton->setDefaultMatrix(coor, sliderSize, 0.0f);
	sliderButton->setHoverMatrix(coor, sliderSize, 0.0f);

	valueChanged = true;
}
