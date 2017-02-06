#include "cSlider.h"

cSlider::cSlider(cGame *game) : cRenderableGroup(game)
{
	bgButton = new cButton(game);
	cTexturedQuadRenderable *quad = new cTexturedQuadRenderable(game, "resources/ui/slider_bg.png", "resources/default");
	quad->setWorldMatrix(Mat3::identity());
	bgButton->addRenderable(quad);
	addRenderable(bgButton);
	bgButton->setDefaultMatrix(Vec2(0.0f), Vec2(200.0f, 20.0f), 0.0f);
	bgButton->setHoverMatrix(Vec2(0.0f), Vec2(200.0f, 20.0f), 0.0f);
	bgButton->setHitArea(Vec2(-200.0f, -20.0f), Vec2(200.0f, 20.0f));

	sliderButton = new cButton(game);
	quad = new cTexturedQuadRenderable(game, "resources/ui/slider_fg.png", "resources/default");
	quad->setWorldMatrix(Mat3::identity());
	sliderButton->addRenderable(quad);
	sliderButton->setDefaultMatrix(Vec2(0.0f), Vec2(20.0f), 0.0f);
	sliderButton->setHoverMatrix(Vec2(0.0f), Vec2(20.0f), 0.0f);
	sliderButton->setHitArea(Vec2(-50.0f), Vec2(50.0f));
	sliderButton->setHoverSpeed(10.0f);
	addRenderable(sliderButton);

	minPos = -200.0f;
	maxPos = 200.0f;
	pressed = false;
}

void cSlider::check(const Vec2& mousePos)
{
	Vec2 relativeMouse = mousePos;
	if (alignment == RenderableAlignment::topLeft)
	{
		relativeMouse -= Vec2(0.0f, (float)game->getScreenDimensions().h);
	}
	else if (alignment == RenderableAlignment::center)
	{
		relativeMouse -= Vec2(game->getScreenDimensions().w * 0.5f, game->getScreenDimensions().h * 0.5f);
	}


	if (pressed)
	{
		float pos = relativeMouse.x - pressShift;
		clamp(pos, minPos, maxPos);
		sliderButton->setDefaultMatrix(Vec2(pos, 0.0f), Vec2(20.0f), 0.0f);
		sliderButton->setHoverMatrix(Vec2(pos, 0.0f), Vec2(20.0f), 0.0f);
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
			clamp(pos, minPos, maxPos);
			sliderButton->setDefaultMatrix(Vec2(pos, 0.0f), Vec2(20.0f), 0.0f);
			sliderButton->setHoverMatrix(Vec2(pos, 0.0f), Vec2(20.0f), 0.0f);
		}
	}
	if (input.isKeyDown(mouse_button_left) == false)
	{
		pressed = false;
	}

}
