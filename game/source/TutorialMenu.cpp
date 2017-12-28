#pragma once


#include "TutorialMenu.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "cTextRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cButton.h"
#include "cTickBox.h"
#include "MissionController.h"
#include "BloodworksControls.h"

#define SHOW_SPEED 10.0f

TutorialMenu::TutorialMenu(Bloodworks* bloodworks)
{
	this->bloodworks = bloodworks;

	window = new cRenderableContainer(bloodworks);
	window->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *bg = new cTexturedQuadRenderable(bloodworks, "resources/pause_bg.png", "resources/default");
	bg->setWorldMatrix(Mat3::scaleMatrix(bg->getTexture()->getDimensions().toVec() * 0.55f).translateBy(0.0f, -10.0f));
	window->addRenderable(bg);

	cTextRenderable* text;

	float t = -35.0f;
	float t2 = 55.0f;

	Vec2 bgSize(50.0f, 16.0f);
	float fontSize = 22.0f;


	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Play Tutorial First?", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(0, 24));
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	window->addRenderable(text);



	yes = new cButton(bloodworks);
	yes->setDefaultMatrix(Vec2(-t2, t), 1.0f, 0.0f);
	yes->setHoverMatrix(Vec2(-t2, t), 1.2f, 0.0f);
	yes->setHitArea(-bgSize, bgSize);
	yes->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Yes", fontSize);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	yes->addRenderable(text);
	yes->setHoverSpeed(8.0f);

	window->addRenderable(yes);

	no = new cButton(bloodworks);
	no->setDefaultMatrix(Vec2(t2, t), 1.0f, 0.0f);
	no->setHoverMatrix(Vec2(t2, t), 1.2f, 0.0f);
	no->setHitArea(-bgSize, bgSize);
	no->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "No", fontSize);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	no->addRenderable(text);
	no->setHoverSpeed(8.0f);

	window->addRenderable(no);

	bloodworks->addRenderable(window, GUI + 119);

	window->setVisible(false);
	alpha = 0.0f;

	lastMouseMoveTimer = 0.0f;
	hoverLevelupPerkIndex = 0;
}

TutorialMenu::~TutorialMenu()
{
	SAFE_DELETE(window);
}

void TutorialMenu::tick()
{
	if (window->isVisible())
	{
		if (alpha < 1.0f)
		{
			alpha += timer.getNonSlowedDt() * SHOW_SPEED;
			alpha = min(alpha, 1.0f);
		}

		cButton* buttons[2] = { yes, no };

		if (input.getDeltaMousePos().lengthSquared() > 0.0f)
		{
			lastMouseMoveTimer = 1.0f;
			
			yes->setEnforcedHovering(cButton::no_enforce);
			no->setEnforcedHovering(cButton::no_enforce);
		}

		lastMouseMoveTimer -= timer.getNonSlowedDt();

		if (joyPadFree)
		{
			if (input.getJoystickAxisPos().x > 0.5f || mapper.isKeyPressed(GameKey::Left))
			{
				hoverLevelupPerkIndex--;
				if (hoverLevelupPerkIndex < 0)
				{
					hoverLevelupPerkIndex += 2;
				}
				joyPadFree = false;
			}
			else if (input.getJoystickAxisPos().x < -0.5f || mapper.isKeyPressed(GameKey::Right))
			{
				hoverLevelupPerkIndex++;
				if (hoverLevelupPerkIndex >= 2)
				{
					hoverLevelupPerkIndex -= 2;
				}
				joyPadFree = false;
			}

			if (joyPadFree == false)
			{
				for (int i = 0; i < 2; i++)
				{
					buttons[i]->setEnforcedHovering(i == hoverLevelupPerkIndex ? cButton::enforce_hovering : cButton::enforce_not_hovering);
				}
			}
		}
		else
		{
			if (fabs(input.getJoystickAxisPos().x) < 0.4f && mapper.isKeyUp(GameKey::Left) && mapper.isKeyUp(GameKey::Right))
			{
				joyPadFree = true;
				lastMouseMoveTimer = 1.0f;
			}
		}

		if (lastMouseMoveTimer < 0.0f && hoverLevelupPerkIndex == -1)
		{
			joyPadFree = false;
			hoverLevelupPerkIndex = -1;
		}

		window->setColor(Vec4(1.0f, 1.0f, 1.0f, alpha) );

		yes->check(input.getMousePos());
		no->check(input.getMousePos());

		if (yes->isClicked() || (hoverLevelupPerkIndex == 0 && mapper.isKeyPressed(GameKey::Select)))
		{
			bloodworks->loadMission("Tutorial");
			window->setVisible(false);
		}

		if (no->isClicked() || (hoverLevelupPerkIndex == 1 && mapper.isKeyPressed(GameKey::Select)))
		{
			bloodworks->loadMission("Survival");
			window->setVisible(false);
		}

		if (mapper.isKeyPressed(GameKey::Back))
		{
			window->setVisible(false);
			mapper.clearKeyPress(GameKey::Back);
		}
	}
}

bool TutorialMenu::isVisible() const
{
	return window->isVisible();
}

void TutorialMenu::show()
{
	window->setVisible(true);
}
