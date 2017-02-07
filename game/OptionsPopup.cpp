#include "OptionsPopup.h"
#include "Bloodworks.h"
#include "cRenderable.h"
#include "cTexture.h"
#include "cFont.h"
#include "cButton.h"

OptionsPopup::OptionsPopup(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	optionsGroup = new cRenderableGroup(bloodworks);
	optionsGroup->setWorldMatrix(Mat3::identity());
	optionsGroup->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.65f));
	optionsGroup->addRenderable(t);
	
	const float titleY = 150.0f;
	const float titleX = 200.0f;

	cTextRenderable* text;
	const float titleButtonSize = 80.0f;

	// gameplay
	gameplayTitle = new cButton(bloodworks);
	gameplayTitle->setAlignment(RenderableAlignment::center);
	gameplayTitle->setDefaultMatrix(Vec2(-titleX, titleY), Vec2(1.0f), 0.0f);
	gameplayTitle->setHoverMatrix(Vec2(-titleX, titleY), Vec2(1.3f), 0.0f);
	gameplayTitle->setHitArea(Vec2(-titleButtonSize, -20.0f), Vec2(titleButtonSize, 20.0f));
	gameplayTitle->setHoverSpeed(10.0f);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Game-Play", 24.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAllignment(TextAlignment::center);
	gameplayTitle->addRenderable(text);
	optionsGroup->addRenderable(gameplayTitle);

	gameplayGroup = new cRenderableGroup(bloodworks);
	gameplayGroup->setVisible(false);

	t = new cTexturedQuadRenderable(bloodworks, "resources/crosshair.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.65f));
	gameplayGroup->addRenderable(t);

	optionsGroup->addRenderable(gameplayGroup);

	// input
	inputTitle = new cButton(bloodworks);
	inputTitle->setAlignment(RenderableAlignment::center);
	inputTitle->setDefaultMatrix(Vec2(0.0f, titleY), Vec2(1.0f), 0.0f);
	inputTitle->setHoverMatrix(Vec2(0.0f, titleY), Vec2(1.3f), 0.0f);
	inputTitle->setHitArea(Vec2(-titleButtonSize, -20.0f), Vec2(titleButtonSize, 20.0f));
	inputTitle->setHoverSpeed(10.0f);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Input", 24.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAllignment(TextAlignment::center);
	inputTitle->addRenderable(text);
	optionsGroup->addRenderable(inputTitle);

	inputGroup = new cRenderableGroup(bloodworks);
	inputGroup->setVisible(false);

	t = new cTexturedQuadRenderable(bloodworks, "resources/reload_ring.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.65f));
	inputGroup->addRenderable(t);

	optionsGroup->addRenderable(inputGroup);

	// audio video
	audioVideoTitle = new cButton(bloodworks);
	audioVideoTitle->setAlignment(RenderableAlignment::center);
	audioVideoTitle->setDefaultMatrix(Vec2(titleX, titleY), Vec2(1.0f), 0.0f);
	audioVideoTitle->setHoverMatrix(Vec2(titleX, titleY), Vec2(1.3f), 0.0f);
	audioVideoTitle->setHitArea(Vec2(-titleButtonSize, -20.0f), Vec2(titleButtonSize, 20.0f));
	audioVideoTitle->setHoverSpeed(10.0f);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Audio-Video", 24.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAllignment(TextAlignment::center);
	audioVideoTitle->addRenderable(text);
	optionsGroup->addRenderable(audioVideoTitle);

	audioVideoGroup = new cRenderableGroup(bloodworks);
	audioVideoGroup->setVisible(false);

	t = new cTexturedQuadRenderable(bloodworks, "resources/crosshair_spread.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.65f));
	audioVideoGroup->addRenderable(t);

	optionsGroup->addRenderable(audioVideoGroup);

	//
	bloodworks->addRenderable(optionsGroup, GUI);

	lastClickedTitle = nullptr;
	prevClickedGroup = nullptr;

	changeTab(gameplayTitle, gameplayGroup);
	lastClickTime = -10.0f;
	tick();
}

OptionsPopup::~OptionsPopup()
{
	SAFE_DELETE(optionsGroup);
}

void OptionsPopup::tick()
{
	gameplayTitle->check(input.getMousePos());
	inputTitle->check(input.getMousePos());
	audioVideoTitle->check(input.getMousePos());

	if (changingTabs == false)
	{
		if (gameplayTitle->isClicked() && lastClickedTitle != gameplayTitle)
		{
			changeTab(gameplayTitle, gameplayGroup);
		}
		if (inputTitle->isClicked() && lastClickedTitle != inputTitle)
		{
			changeTab(inputTitle, inputGroup);
		}
		if (audioVideoTitle->isClicked() && lastClickedTitle != audioVideoTitle)
		{
			changeTab(audioVideoTitle, audioVideoGroup);
		}
	}

	if (changingTabs)
	{
		float clickT = (timer.getRealTime() - lastClickTime) / 0.2f;
		if (clickT > 1.0f)
		{
			clickT = 1.0f;
			changingTabs = false;
		}
		lastClickedTitle->setDefaultScale(1.3f + clickT * 0.3f);
		lastClickedGroup->setColor(Vec4(1.0f, 1.0f, 1.0f, clickT));
		if (prevClickedTitle != nullptr)
		{
			if (changingTabs == false)
			{
				prevClickedGroup->setVisible(false);
			}
			prevClickedTitle->setHoverScale(1.3f + (1.0f - clickT) * 0.3f);
			prevClickedGroup->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f - clickT));
		}
	}
}

void OptionsPopup::changeTab(cButton *tab, cRenderableGroup *group)
{
	if (lastClickedTitle != nullptr)
	{
		lastClickedTitle->setDefaultScale(1.0f);
		lastClickedTitle->setHoverWeight(1.0f);
		lastClickedTitle->setEnforcedHovering(cButton::no_enforce);
	}
	prevClickedGroup = lastClickedGroup;
	prevClickedTitle = lastClickedTitle;
	lastClickedTitle = tab;
	lastClickedGroup = group;
	lastClickedGroup->setVisible(true);
	lastClickedTitle->setDefaultScale(1.3f);
	lastClickedTitle->setEnforcedHovering(cButton::enforce_not_hovering);
	lastClickTime = timer.getRealTime();
	changingTabs = true;
}
