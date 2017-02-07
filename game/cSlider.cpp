#include "cSlider.h"

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
}

void cSlider::check(const Vec2& mousePos)
{
	Vec2 relativeMouse = game->getRelativeMousePos(mousePos, getAlignment());

	float maxShift = bgSize.x - sliderSize.x - edgeShift;

	if (pressed)
	{
		float pos = relativeMouse.x - pressShift;
		clamp(pos, -maxShift, maxShift);
		setSliderPos(pos);
	}

	bgButton->check(mousePos);
	sliderButton->check(mousePos);

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

void cSlider::setSliderPos(float pos)
{
	sliderButton->setDefaultMatrix(Vec2(pos, 0.0f), sliderSize, 0.0f);
	sliderButton->setHoverMatrix(Vec2(pos, 0.0f), sliderSize, 0.0f);
}
