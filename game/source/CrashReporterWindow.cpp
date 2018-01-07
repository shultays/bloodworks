#pragma once


#include "CrashReporterWindow.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "cTextRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cButton.h"
#include "cTickBox.h"

CrashReportWindow::CrashReportWindow(Bloodworks* bloodworks)
{
	this->bloodworks = bloodworks;

	input.showMouse();

	window = new cRenderableContainer(bloodworks);
	window->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.55f));
	window->addRenderable(t);

	cTextRenderable* text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "We Have a Problem", 24.0f);
	text->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, 110.0f)));
	text->setTextAlignment(TextAlignment::center);
	window->addRenderable(text);


	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "It looks like there was an unexpected termination of the game on your previous run (possible due to a crash). \n\nDo you want to send a report to help the developer?", 18.0f);
	text->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, 0.0f)));
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	text->setMaxLength(500.0f);
	window->addRenderable(text);


	float shift = 120.0f;

	send = new cButton(bloodworks);
	cTexturedQuadRenderable *quad = new cTexturedQuadRenderable(bloodworks, "resources/ui/slider_bg.png", "resources/default");
	Vec2 bgSize = quad->getTexture()->getDimensions().toVec();
	quad->setWorldMatrix(Mat3::scaleMatrix(bgSize));
	send->addRenderable(quad);
	send->setDefaultMatrix(Vec2(-shift, -150.0f), 1.0f, 0.0f);
	send->setHoverMatrix(Vec2(-shift, -150.0f), 1.1f, 0.0f);
	send->setHitArea(-bgSize, bgSize);

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Send (Enter)", 18.0f);
	text->setWorldMatrix(Mat3::translationMatrix(0.0f, bgSize.h * 0.7f));
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	send->addRenderable(text);
	send->setHoverSpeed(8.0f);

	window->addRenderable(send);

	cancel = new cButton(bloodworks);
	quad = new cTexturedQuadRenderable(bloodworks, "resources/ui/slider_bg.png", "resources/default");
	bgSize = quad->getTexture()->getDimensions().toVec();
	quad->setWorldMatrix(Mat3::scaleMatrix(bgSize));
	cancel->addRenderable(quad);
	cancel->setDefaultMatrix(Vec2(shift, -150.0f), 1.0f, 0.0f);
	cancel->setHoverMatrix(Vec2(shift, -150.0f), 1.1f, 0.0f);
	cancel->setHitArea(-bgSize, bgSize);
	cancel->setHoverSpeed(8.0f);

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Cancel (Esc)", 18.0f);
	text->setWorldMatrix(Mat3::translationMatrix(0.0f, bgSize.h * 0.7f));
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	cancel->addRenderable(text);

	window->addRenderable(cancel);


	Vec2 p(55.0f, -110.0f);
	remember = new cTickBox(bloodworks, "");
	remember->setWorldMatrix(Mat3::translationMatrix(p));
	remember->setDefaultMatrix(p, Vec2(20.0f), 0.0f);
	remember->setHoverMatrix(p, Vec2(20.0f), 0.0f);
	remember->setHitArea(-20.0f * 0.6f, 20.0f * 0.6f);
	remember->setChecked(false);
	window->addRenderable(remember);

	bloodworks->addRenderable(window, GUI + 100);

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Remember:", 18.0f);
	text->setWorldMatrix(Mat3::translationMatrix(Vec2(p.x - 75.0f, p.y + 5.0f)));
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	text->setMaxLength(500.0f);
	window->addRenderable(text);

	closed = false;

	shouldSend = true;
}

CrashReportWindow::~CrashReportWindow()
{
	SAFE_DELETE(window);
}

void CrashReportWindow::tick()
{
	send->check(input.getMousePos());
	cancel->check(input.getMousePos());
	remember->check(input.getMousePos());

	if (send->isClicked() || cancel->isClicked() || input.isKeyDown(key_return) || input.isKeyDown(key_escape))
	{
		remeberChoice = remember->isChecked();
		shouldSend = send->isClicked() || input.isKeyDown(key_return);
		closed = true;
	}
}

void CrashReportWindow::windowResized()
{

}
