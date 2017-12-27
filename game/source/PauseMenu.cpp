#pragma once


#include "PauseMenu.h"
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

PauseMenu::PauseMenu(Bloodworks* bloodworks)
{
	this->bloodworks = bloodworks;

	window = new cRenderableContainer(bloodworks);
	window->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *bg = new cTexturedQuadRenderable(bloodworks, "resources/pause_bg.png", "resources/default");
	bg->setWorldMatrix(Mat3::scaleMatrix(bg->getTexture()->getDimensions().toVec() * 0.55f).translateBy(0.0f, -10.0f));
	window->addRenderable(bg);

	cTextRenderable* text;

	float t = 35.0f;
	Vec2 bgSize(150.0f, 16.0f);
	float fontSize = 22.0f;

	restart = new cButton(bloodworks);
	restart->setDefaultMatrix(Vec2(0.0f, t), 1.0f, 0.0f);
	restart->setHoverMatrix(Vec2(0.0f, t), 1.2f, 0.0f);
	restart->setHitArea(-bgSize, bgSize);
	restart->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Restart", fontSize);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	restart->addRenderable(text);
	restart->setHoverSpeed(8.0f);

	window->addRenderable(restart);

	options = new cButton(bloodworks);
	options->setDefaultMatrix(Vec2(0.0f, 0), 1.0f, 0.0f);
	options->setHoverMatrix(Vec2(0.0f, 0), 1.2f, 0.0f);
	options->setHitArea(-bgSize, bgSize);
	options->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Options", fontSize);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	options->addRenderable(text);
	options->setHoverSpeed(8.0f);

	window->addRenderable(options);

	quit = new cButton(bloodworks);
	quit->setDefaultMatrix(Vec2(0.0f, -t), 1.0f, 0.0f);
	quit->setHoverMatrix(Vec2(0.0f, -t), 1.2f, 0.0f);
	quit->setHitArea(-bgSize, bgSize);

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Quit", fontSize);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	quit->addRenderable(text);
	quit->setHoverSpeed(8.0f);
	quit->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));

	window->addRenderable(quit);

	window->setVisible(false);
	bloodworks->addRenderable(window, GUI + 119);

	alpha = 0.0f;
}

PauseMenu::~PauseMenu()
{
	SAFE_DELETE(window);
}

void PauseMenu::tick()
{
	bool show = bloodworks->isPaused() && bloodworks->getMissionController()->isLoaded() 
		&& !bloodworks->isLevelUpPopupVisible() && !bloodworks->isOptionsVisible();

	if (show)
	{
		if (alpha < 1.0f)
		{
			alpha += timer.getNonSlowedDt() * SHOW_SPEED;
			alpha = min(alpha, 1.0f);
		}

		if (!window->isVisible())
		{
			window->setVisible(true);
			input.showMouse();
			hoverLevelupPerkIndex = -1;
			joyPadFree = false;
			lastMouseMoveTimer = 0.0f;
		}
	}
	else
	{
		if (alpha > 0.0f && !bloodworks->isOptionsVisible())
		{
			alpha -= timer.getNonSlowedDt() * SHOW_SPEED;
			alpha = max(alpha, 0.0f);
		}

		if (window->isVisible())
		{
			if (alpha < 0.01f)
			{
				window->setVisible(false);
			}
			if (!bloodworks->isOptionsVisible())
			{
				input.hideMouse();
			}
		}
	}


	cButton* buttons[3] = { restart, options, quit };

	if (window->isVisible())
	{

		if (input.getDeltaMousePos().lengthSquared() > 0.0f)
		{
			lastMouseMoveTimer = 1.0f;
			
			restart->setEnforcedHovering(cButton::no_enforce);
			options->setEnforcedHovering(cButton::no_enforce);
			quit->setEnforcedHovering(cButton::no_enforce);
		}

		lastMouseMoveTimer -= timer.getNonSlowedDt();

		if (joyPadFree)
		{
			if (input.getJoystickAxisPos().x > 0.5f || mapper.isKeyPressed(GameKey::Up))
			{
				hoverLevelupPerkIndex--;
				if (hoverLevelupPerkIndex < 0)
				{
					hoverLevelupPerkIndex += 3;
				}
				joyPadFree = false;
			}
			else if (input.getJoystickAxisPos().x < -0.5f || mapper.isKeyPressed(GameKey::Down))
			{
				hoverLevelupPerkIndex++;
				if (hoverLevelupPerkIndex >= 3)
				{
					hoverLevelupPerkIndex -= 3;
				}
				joyPadFree = false;
			}

			if (joyPadFree == false)
			{
				for (int i = 0; i < 3; i++)
				{
					buttons[i]->setEnforcedHovering(i == hoverLevelupPerkIndex ? cButton::enforce_hovering : cButton::enforce_not_hovering);
				}
			}
		}
		else
		{
			if (fabs(input.getJoystickAxisPos().x) < 0.4f && mapper.isKeyUp(GameKey::Up) && mapper.isKeyUp(GameKey::Down))
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

		if (!bloodworks->isOptionsVisible())
		{
			restart->check(input.getMousePos());
			quit->check(input.getMousePos());
			options->check(input.getMousePos());

			if (restart->isClicked() || (hoverLevelupPerkIndex == 0 && mapper.isKeyPressed(GameKey::Select)) )
			{
				bloodworks->doUnpause(true);
				window->setVisible(false);
				alpha = 0.0f;
				bloodworks->restartMission();
			}

			if (options->isClicked() || (hoverLevelupPerkIndex == 1 && mapper.isKeyPressed(GameKey::Select)))
			{
				bloodworks->showOptions();
			}

			if (quit->isClicked() || (hoverLevelupPerkIndex == 0 && mapper.isKeyPressed(GameKey::Select)))
			{
				bloodworks->doUnpause( true );
				bloodworks->gotoMainMenu();
				window->setVisible(false);
				alpha = 0.0f;
			}
		}
	}

}
