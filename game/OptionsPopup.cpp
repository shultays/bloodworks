#include "OptionsPopup.h"
#include "Bloodworks.h"
#include "cRenderable.h"
#include "cTexture.h"
#include "cFont.h"
#include "cButton.h"
#include "cTickBox.h"
#include "cSlider.h"
#include "cPersistent.h"

OptionsPopup::OptionsPopup(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	cPersistent* config = bloodworks->getConfig();

	optionsGroup = new cRenderableGroup(bloodworks);
	optionsGroup->setWorldMatrix(Mat3::identity());
	optionsGroup->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.65f));
	optionsGroup->addRenderable(t);
	
	const float titleY = 150.0f;
	const float titleX = 200.0f;

	cTextRenderable* text;
	const float titleButtonSize = 75.0f;

	float x, y;

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

	float tickSize = 25.0f;
	float fontSize = 23.0f;
	float tickShift = 380.0f;
	float rowShift = 45.0f;
	float sliderShift = 290.0f;

	x = -200.0f;
	y = 40.0f;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Gore", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);

	gore = new cTickBox(bloodworks);
	gore->setWorldMatrix(Mat3::translationMatrix(x, y));
	gore->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	gore->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	gore->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	gameplayGroup->addRenderable(gore);

	gore->setChecked(config->getInt("gore", 1) != 0);
	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Screen Shake", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);

	screenShake = new cTickBox(bloodworks);
	screenShake->setWorldMatrix(Mat3::translationMatrix(x, y));
	screenShake->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	screenShake->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	screenShake->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	screenShake->setChecked(config->getInt("screen_shake", 1) != 0);
	gameplayGroup->addRenderable(screenShake);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Lock Crosshair", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);

	lockCrosshair = new cTickBox(bloodworks);
	lockCrosshair->setWorldMatrix(Mat3::translationMatrix(x, y));
	lockCrosshair->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	lockCrosshair->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	lockCrosshair->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	lockCrosshair->setChecked(config->getInt("lock_crosshair", 1) != 0);
	gameplayGroup->addRenderable(lockCrosshair);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Auto Open Perk PopUp", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);

	autoLevelUp = new cTickBox(bloodworks);
	autoLevelUp->setWorldMatrix(Mat3::translationMatrix(x, y));
	autoLevelUp->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	autoLevelUp->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	autoLevelUp->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	autoLevelUp->setChecked(config->getInt("auto_open_perk_popup", 1) != 0);
	gameplayGroup->addRenderable(autoLevelUp);


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

	x = -240.0f;
	y = 0.0f;
	tickShift = 0.0f;
	sliderShift = 390.0f;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Sensitivity", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	inputGroup->addRenderable(text);

	sensitivity = new cSlider(bloodworks);
	sensitivity->setWorldMatrix(Mat3::translationMatrix(x + sliderShift, y - 5.0f));
	sensitivity->setMinMax(0, 100);
	sensitivity->setValue(config->getInt("sensitivity", 50));
	inputGroup->addRenderable(sensitivity);

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

	x = -200.0f;
	y = 40.0f;
	sliderShift = 290.0f;
	tickShift = 380.0f;

	//text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Resolution", fontSize);
	//text->setWorldMatrix(Mat3::translationMatrix(x, y));
	//text->setTextAllignment(TextAlignment::left);
	//text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	//audioVideoGroup->addRenderable(text);
	//
	//y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Full Screen", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	fullScreen = new cTickBox(bloodworks);
	fullScreen->setWorldMatrix(Mat3::translationMatrix(x, y));
	fullScreen->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	fullScreen->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	fullScreen->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	fullScreen->setChecked(config->getInt("full_screen", 0) != 0);
	audioVideoGroup->addRenderable(fullScreen);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "V-Sync", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	vsync = new cTickBox(bloodworks);
	vsync->setWorldMatrix(Mat3::translationMatrix(x, y));
	vsync->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	vsync->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	vsync->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	vsync->setChecked(config->getInt("vsync", 0) != 0);
	audioVideoGroup->addRenderable(vsync);

	y -= rowShift * 1.5f;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Volume", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	volume = new cSlider(bloodworks);
	volume->setWorldMatrix(Mat3::translationMatrix(x + sliderShift, y - 5.0f));
	volume->setMinMax(0, 100);
	volume->setValue(config->getInt("volume", 50));
	audioVideoGroup->addRenderable(volume);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Music", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAllignment(TextAlignment::left);
	text->setVerticalTextAllignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	musicVolume = new cSlider(bloodworks);
	musicVolume->setWorldMatrix(Mat3::translationMatrix(x + sliderShift, y - 5.0f));
	musicVolume->setMinMax(0, 100);
	musicVolume->setValue(config->getInt("music_volume", 50));
	audioVideoGroup->addRenderable(musicVolume);


	optionsGroup->addRenderable(audioVideoGroup);

	//
	bloodworks->addRenderable(optionsGroup, GUI + 22);

	lastClickedTitle = nullptr;
	prevClickedGroup = nullptr;

	changeTab(gameplayTitle, gameplayGroup);
	lastClickTime = -10.0f;
	tick();
	optionsGroup->setVisible(false);
}

OptionsPopup::~OptionsPopup()
{
	SAFE_DELETE(optionsGroup);
}

bool OptionsPopup::isVisible() const
{
	return optionsGroup->isVisible();
}

void OptionsPopup::show()
{
	optionsGroup->setVisible(true);
}

void OptionsPopup::tick()
{
	if (isVisible() == false)
	{
		return;
	}
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
		lastClickedTitle->setDefaultScale(1.3f + clickT * 0.2f);
		lastClickedGroup->setColor(Vec4(1.0f, 1.0f, 1.0f, clickT));
		if (prevClickedTitle != nullptr)
		{
			if (changingTabs == false)
			{
				prevClickedGroup->setVisible(false);
			}
			prevClickedTitle->setHoverScale(1.3f + (1.0f - clickT) * 0.2f);
			prevClickedGroup->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f - clickT));
		}
	}

	cPersistent* config = bloodworks->getConfig();

	if (lastClickedTitle == gameplayTitle)
	{
		gore->check(input.getMousePos());
		screenShake->check(input.getMousePos());
		lockCrosshair->check(input.getMousePos());

		if (gore->isChanged())
		{
			config->set("gore", gore->isChecked() ? 1 : 0);
		}
		if (screenShake->isChanged())
		{
			config->set("screen_shake", screenShake->isChecked() ? 1 : 0);
		}
		if (lockCrosshair->isChanged())
		{
			config->set("lock_crosshair", lockCrosshair->isChecked() ? 1 : 0);
		}
	}

	if (lastClickedTitle == inputTitle)
	{
		sensitivity->check(input.getMousePos());

		if (sensitivity->isChanged())
		{
			config->set("sensitivity", sensitivity->getIntValue());
		}
	}

	if (lastClickedTitle == audioVideoTitle)
	{
		fullScreen->check(input.getMousePos());
		vsync->check(input.getMousePos());
		volume->check(input.getMousePos());
		musicVolume->check(input.getMousePos());

		if (fullScreen->isChanged())
		{
			config->set("full_screen", fullScreen->isChecked() ? 1 : 0);
		}

		if (vsync->isChanged())
		{
			config->set("vsync", vsync->isChecked() ? 1 : 0);
		}

		if (volume->isChanged())
		{
			config->set("volume", volume->getIntValue());
		}
		if (musicVolume->isChanged())
		{
			config->set("music_volume", musicVolume->getIntValue());
		}
	}

	if (input.isKeyPressed(key_escape) || input.isKeyPressed(joystick_0_button_back))
	{
		optionsGroup->setVisible(false);
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
