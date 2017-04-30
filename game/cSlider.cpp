#include "cSlider.h"
#include "cButton.h"

cSlider::cSlider(cGame *game) : cRenderableGroup(game)
{
	bgButton = new cButton(game);
	cTexturedQuadRenderable *quad = new cTexturedQuadRenderable(game, "resources/ui/slider_bg.png", "resources/default");
	bgSize = quad->getTexture()->getDimensions().toVec();
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

void cSlider::check(const Vec2& mousePos)
{
	Vec2 shiftedPos = mousePos - getPosition();
	Vec2 relativeMouse = game->getRelativeMousePos(shiftedPos, getAlignment());

	float maxShift = bgSize.x - sliderSize.x - edgeShift;

	if (pressed)
	{
		float pos = relativeMouse.x - pressShift;
		clamp(pos, -maxShift, maxShift);
		setSliderPos(pos);
	}

	bgButton->check(shiftedPos);
	sliderButton->check(shiftedPos);

	if (sliderButton->isPressed())
	{
		pressShift = relativeMouse.x - sliderButton->getPosition().x;
		pressed = true;
	}

	if (bgButton->isReleased())
	{
		if (pressed == false)
		{
			float pos = relativeMouse.x;
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
	if (isFloat)
	{
		setValue((float)value);
		return;
	}
	curValue = value;
	clamp(curValue, minValue, maxValue);
	float maxShift = bgSize.x - sliderSize.x - edgeShift;
	float t = ((float)value - minValue) / (maxValue - minValue);
	float pos = -maxShift + maxShift * 2.0f * t;
	setSliderPos(pos);
}

void cSlider::setValue(float value)
{
	if (isFloat == false)
	{
		setValue((int)round(value));
		return;
	}

	curValueF = value;
	clamp(curValueF, minValueF, maxValueF);
	float maxShift = bgSize.x - sliderSize.x - edgeShift;
	float t = (value - minValueF) / (maxValueF - minValueF);
	float pos = -maxShift + maxShift * 2.0f * t;
	setSliderPos(pos);

	valueChanged = false;
}

void cSlider::setSliderPos(float pos)
{
	float maxShift = bgSize.x - sliderSize.x - edgeShift;
	float t = (pos + maxShift) / (2 * maxShift);
	if (isFloat == false)
	{
		int count = maxValue - minValue;
		int r = (int)round(t * count);
		curValue = r - minValue;
		t = ((float)r) / count;
		pos = -maxShift + maxShift * 2.0f * t;
	}

	sliderButton->setDefaultMatrix(Vec2(pos, 0.0f), sliderSize, 0.0f);
	sliderButton->setHoverMatrix(Vec2(pos, 0.0f), sliderSize, 0.0f);

	valueChanged = true;
}
