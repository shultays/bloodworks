#pragma once


#include "CreditsWindow.h"
#include "cRenderableContainer.h"
#include "cTextRenderable.h"
#include "Bloodworks.h"
#include "BloodworksControls.h"
#include "cTexturedQuadRenderable.h"
#include "cShader.h"

CreditsWindow::CreditsWindow(Bloodworks* bloodworks)
{
	this->bloodworks = bloodworks;

	window = new cRenderableContainer(bloodworks);
	window->setAlignment(RenderableAlignment::center);
	window->setVisible(false);

	bg = new cTexturedQuadRenderable(bloodworks, "resources/black.png", "resources/credits");
	bg->setWorldMatrix(Mat3::scaleMatrix(10000.0f));
	bg->setUniform(bg->getShader()->addUniform("uOutsideAlpha", TypeFloat).index, 0.5f);
	bg->setUniform(bg->getShader()->addUniform("uScreenLimits", TypeVec2).index, Vec2(150.0f, 150.0f));
	bg->setColor(Vec4::fromColor(0));
	window->addRenderable(bg);

	credits = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 24.0f);
	credits->setTextAlignment(TextAlignment::center);
	credits->setVerticalTextAlignment(VerticalTextAlignment::bottom);
	credits->setShader(resources.getShader("resources/credits"));
	credits->setUniform(bg->getShader()->addUniform("uOutsideAlpha", TypeFloat).index, 0.0f);
	credits->setUniform(bg->getShader()->addUniform("uScreenLimits", TypeVec2).index, Vec2(250.0f, 350.0f));
	window->addRenderable(credits);

	std::string text;
	textFileRead("thanks.txt", text);
	credits->setText(text);

	bloodworks->addRenderable(window, GUI + 100);
}

CreditsWindow::~CreditsWindow()
{
	SAFE_DELETE(window);
}

void CreditsWindow::tick()
{
	if (bloodworks->isFirstTick())
	{
		out << "CreditsWindow::tick\n";
	}
	if (isVisible() == false)
	{
		return;
	}

	Vec2 screenSize = bloodworks->getScreenDimensions().toVec();
	credits->setUniform(bg->getShader()->addUniform("uScreenSize", TypeVec2).index, screenSize);
	bg->setUniform(bg->getShader()->addUniform("uScreenSize", TypeVec2).index, screenSize);
	credits->setMaxLength(min(screenSize.x * 0.9f -150.0f, 800.0f));
	float a = (timer.getTime() - visibleTime) * 20.0f;
	saturate(a);
	bg->setColor(Vec4(1.0f, 1.0f, 1.0f, a));

	float speed = 40.0f;
	if (screenSize.y > 800)
	{
		speed = 60.0f;
	}
	else if (screenSize.y > 600)
	{
		speed = 40 + 20 * (screenSize.y - 600) / 200.0f;
	}
	textY += timer.getDt() * speed;
	if (textY > 4000.0f + screenSize.y)
	{
		resetTextPos();
	}
	credits->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, textY)));

	if (input.isKeyPressed(mouse_button_left) || mapper.isKeyPressed(GameKey::Back))
	{
		mapper.clearKeyPress(GameKey::Back);
		input.clearKeyPress(mouse_button_left);
		setVisible(false);
	}
}

void CreditsWindow::setVisible(bool visible)
{
	if (visible)
	{
		visibleTime = timer.getTime();
		resetTextPos();
		credits->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, textY)));
	}
	window->setVisible(visible);
}

bool CreditsWindow::isVisible() const
{
	return window->isVisible();
}

void CreditsWindow::resetTextPos()
{
	textY = -450.0f;
	float y = bloodworks->getScreenDimensions()[1] * 0.5f - 50.0f;
	textY = max(textY, -y);
}
