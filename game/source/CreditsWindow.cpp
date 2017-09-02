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


	credits->setText(
		
		"BLOODWORKS" "\n\n"

		"Developed by Engin Mercan" "\n\n"

		"Huge thanks to artists that made their work available for free and made this game possible!" "\n\n"

		"Main Menu Music by" "\n"
		"Clearside" "\n"
		"www.clearsidemusic.com" "\n"
		"\n"

		"In-Game Music" "\n"
		"Matthew Pablo" "\n"
		"www.matthewpablo.com" "\n"
		"\n"

		"Title Font" "\n"
		"ModBlackmoon" "\n"
		"modblackmoon.com" "\n"
		"\n"

		"In-Game Fonts" "\n""\n"
		"Clint Bellanger" "\n"
		"clintbellanger.net" "\n"
		"\n"
		"Robey Pointer" "\n"
		"robey.lag.net" "\n"
		"\n"

		"Window Backgrounds" "\n"
		"kindland" "\n"
		"via opengameart.org" "\n"
		"\n"

		"Player Sprite" "\n"
		"tatermand" "\n"
		"via opengameart.org" "\n"
		"\n"

		"Alien Sprites" "\n"
		"7Soul1" "\n"
		"via deviantart.com" "\n"
		"\n"

		"Spider Sprites" "\n"
		"Ari Feldman" "\n"
		"www.widgetworx.com" "\n"
		"\n"

		"Egg-Sack Model" "\n"
		"Danimal" "\n"
		"via opengameart.org" "\n"
		"\n"

		"Health Bar" "\n"
		"Paul Wortmann" "\n"
		"www.PhysHexGames.com" "\n"
		"\n"

		"Crosshairs" "\n"
		"fluxord" "\n"
		"via opengameart.org" "\n"
		"\n"

		"Blood Decals" "\n"
		"knittel" "\n"
		"via opengameart.org" "\n"
		"\n"

		"Buildings / Bunkers" "\n"
		"chabull" "\n"
		"via opengameart.org" "\n"
		"\n"

		"Ground Textures" "\n"
		"Saroman" "\n"
		"via opengameart.org" "\n"
		"\n"
		"FabooGuy" "\n"
		"via deviantart.com" "\n"
		"\n"
		"demolitiondan" "\n"
		"via deviantart.com" "\n"
		"\n"

		"Weapon Sprites" "\n\n"
		"Drakojan14" "\n"
		"via deviantart.com" "\n"
		"\n"
		"Im-Not-Crying" "\n"
		"via deviantart.com" "\n"
		"\n"
		"unpurecoco" "\n"
		"via basilmarket.com" "\n"
		"\n"
		"sleepy_byte" "\n"
		"via opengameart.org" "\n"
		"\n"
		"Bonsaiheldin" "\n"
		"bonsaiheld.org" "\n"
		"\n"

		"Particles" "\n\n"
		"hackcraft" "\n"
		"hackcraft.de" "\n"
		"\n"
		"tauran" "\n"
		"via opengameart.org" "\n"
		"\n"

		"In-Game Audio Assets" "\n\n"
		"Michael Klier" "\n"
		"via opengameart.org" "\n"
		"\n"
		"Michel Baradari" "\n"
		"via opengameart.org" "\n"
		"\n"
		"Little Robot Sound Factory" "\n"
		"www.littlerobotsoundfactory.com" "\n"
		"\n"
		"Stephen M. Cameron" "\n"
		"via opengameart.org" "\n"
		"\n"
		"Iwan 'qubodup' Gabovitch" "\n"
		"via opengameart.org" "\n"
		"\n"
		"ViRiX Dreamcore (David Mckee)" "\n"
		"www.soundcloud.com/virix" "\n"
		"\n"
		"Guns by Gary" "\n"
		"fossilrecords.net" "\n"
		"\n"

		"And big thanks to you for playing my game!" "\n"
		"\n"
		"Engin Mercan" "\n"
		"mail@enginmercan.com" "\n"
	);


	bloodworks->addRenderable(window, GUI + 100);
}

CreditsWindow::~CreditsWindow()
{
	SAFE_DELETE(window);
}

void CreditsWindow::tick()
{
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
	textY += timer.getDt() * speed * 1.2f;
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
	textY = -350.0f;
	float y = bloodworks->getScreenDimensions()[1] * 0.5f - 50;
	textY = max(textY, -y);
}
