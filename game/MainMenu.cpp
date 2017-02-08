#include "MainMenu.h"
#include "cRenderable.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "cButton.h"
#include "cFont.h"
#include "cSound.h"

MainMenu::MainMenu(Bloodworks *b)
{
	this->bloodworks = b;
	bloodworksText = new cTexturedQuadRenderable(bloodworks, "resources/menu_text.png", "resources/default");
	bloodworksText->setAlignment(RenderableAlignment::top);
	bloodworks->addRenderable(bloodworksText, GUI + 20);

	cSoundSampleShr hover = resources.getSoundSample("resources/sounds/hover.ogg");
	cSoundSampleShr click = resources.getSoundSample("resources/sounds/click.ogg");

	cTextRenderable *text;

	newGame = new cButton(bloodworks);
	newGame->setAlignment(RenderableAlignment::topLeft);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "New Game", 32.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	newGame->addRenderable(text);
	newGame->setHoverSpeed(10.0f);
	newGame->setSounds(click, hover);
	bloodworks->addRenderable(newGame, GUI + 21);

	mods = new cButton(bloodworks);
	mods->setAlignment(RenderableAlignment::topLeft);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Mods", 32.0f, Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	text->setWorldMatrix(Mat3::identity());
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	mods->addRenderable(text);
	mods->setHoverSpeed(10.0f);
	mods->setSounds(click, hover);
	bloodworks->addRenderable(mods, GUI + 21);

	options = new cButton(bloodworks);
	options->setAlignment(RenderableAlignment::topLeft);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Options", 32.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	options->addRenderable(text);
	options->setHoverSpeed(10.0f);
	options->setSounds(click, hover);
	bloodworks->addRenderable(options, GUI + 21);

	credits = new cButton(bloodworks);
	credits->setAlignment(RenderableAlignment::topLeft);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Credits", 32.0f, Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	text->setWorldMatrix(Mat3::identity());
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	credits->addRenderable(text);
	credits->setHoverSpeed(10.0f);
	credits->setSounds(click, hover);
	bloodworks->addRenderable(credits, GUI + 21);

	quit = new cButton(bloodworks);
	quit->setAlignment(RenderableAlignment::topLeft);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Quit Game", 32.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	quit->addRenderable(text);
	quit->setHoverSpeed(10.0f);
	quit->setSounds(click, hover);
	bloodworks->addRenderable(quit, GUI + 21);

	resize();
	music = resources.getSoundSample("resources/sounds/main_menu.ogg");
	setVisible(false);
}

MainMenu::~MainMenu()
{
	SAFE_DELETE(bloodworksText);
	SAFE_DELETE(newGame);
	SAFE_DELETE(mods);
	SAFE_DELETE(options);
	SAFE_DELETE(credits);
	SAFE_DELETE(quit);
}

void MainMenu::resize()
{
	Vec2 gameSize = bloodworks->getScreenDimensions().toVec();
	Vec2 textureSize;

	textureSize = bloodworksText->getTexture()->getDimensions().toVec();
	textureSize *= gameSize.w * 0.95f * 0.5f / textureSize.w;
	bloodworksText->setWorldMatrix(Mat3::scaleMatrix(textureSize).translateBy(0.0f, -textureSize.h - 30.0f));

	float remainingHeight = gameSize.h - (textureSize.h * 2.0f + 100.0f);


	float scale = 1.0f;

	if (remainingHeight > 400.0f)
	{
		scale *= remainingHeight / 400.0f;
	}

	float totalSize = 50.0f * scale * 6.0f;

	Vec2 menuStart(50.0f * scale + (200.0f * (scale - 1.0f)), remainingHeight - gameSize.h - (remainingHeight - totalSize) * 0.5f);

	newGame->setDefaultMatrix(menuStart, Vec2(scale), 0.0f);
	newGame->setHoverMatrix(menuStart + Vec2(-10.0f, 0.0), Vec2(scale * 1.2f), 0.0f);

	menuStart -= Vec2(0.0f, 50.0f * scale);

	mods->setDefaultMatrix(menuStart, Vec2(scale), 0.0f);
	mods->setHoverMatrix(menuStart + Vec2(-10.0f, 0.0), Vec2(scale * 1.2f), 0.0f);

	menuStart -= Vec2(0.0f, 50.0f * scale);

	options->setDefaultMatrix(menuStart, Vec2(scale), 0.0f);
	options->setHoverMatrix(menuStart + Vec2(-10.0f, 0.0), Vec2(scale * 1.2f), 0.0f);

	menuStart -= Vec2(0.0f, 50.0f * scale);

	credits->setDefaultMatrix(menuStart, Vec2(scale), 0.0f);
	credits->setHoverMatrix(menuStart + Vec2(-10.0f, 0.0), Vec2(scale * 1.2f), 0.0f);

	menuStart -= Vec2(0.0f, 50.0f * scale);

	quit->setDefaultMatrix(menuStart, Vec2(scale), 0.0f);
	quit->setHoverMatrix(menuStart + Vec2(-10.0f, 0.0), Vec2(scale * 1.2f), 0.0f);

	float h = 25 * scale;

	newGame->setHitArea(Vec2(-50.0f, -h), Vec2(250.0f * scale, h));
	mods->setHitArea(Vec2(-50.0f, -h), Vec2(120.0f * scale, h));
	options->setHitArea(Vec2(-50.0f, -h), Vec2(200.0f * scale, h));
	credits->setHitArea(Vec2(-50.0f, -h), Vec2(190.0f * scale, h));
	quit->setHitArea(Vec2(-50.0f, -h), Vec2(250.0f * scale, h));
}

void MainMenu::tick(bool hasPopup)
{
	if (bloodworksText->isVisible() == false)
	{
		return;
	}

	int enforceState = hasPopup ? cButton::enforce_not_hovering : cButton::no_enforce;
	newGame->setEnforcedHovering(enforceState);
	mods->setEnforcedHovering(enforceState);
	options->setEnforcedHovering(enforceState);
	credits->setEnforcedHovering(enforceState);
	quit->setEnforcedHovering(enforceState);

	newGame->check(input.getMousePos());
	mods->check(input.getMousePos());
	options->check(input.getMousePos());
	credits->check(input.getMousePos());
	quit->check(input.getMousePos());

	if (hasPopup == false)
	{
		if (newGame->isClicked() || input.isKeyPressed(joystick_0_button_a) || input.isKeyPressed(joystick_0_button_start))
		{
			input.clearKeyPress(joystick_0_button_a);
			input.clearKeyPress(joystick_0_button_start);
			bloodworks->loadMission("Survival");
		}
		else if (options->isClicked())
		{
			bloodworks->showOptions();
		}
		else if (quit->isClicked() || input.isKeyPressed(key_escape) || input.isKeyPressed(joystick_0_button_back))
		{
			coral.quitGame();
		}
	}
}

void MainMenu::setVisible(bool visible)
{
	bloodworksText->setVisible(visible);

	newGame->setVisible(visible);
	mods->setVisible(visible);
	options->setVisible(visible);
	credits->setVisible(visible);
	quit->setVisible(visible);
	if (visible)
	{
		handle = music->play();
		handle.setLooped(true);
		input.showMouse();
		input.setMousePosition(bloodworks->getScreenDimensions().w / 2, bloodworks->getScreenDimensions().h / 2);
	}
	else
	{
		handle.stop();
	}
}
