#include "OptionsPopup.h"
#include "Bloodworks.h"
#include "cRenderable.h"
#include "cScrollContainer.h"
#include "cTextRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "cTexture.h"
#include "cFont.h"
#include "cButton.h"
#include "cTickBox.h"
#include "cSlider.h"
#include "cKeyMapButton.h"
#include "BloodworksControls.h"
#include "BloodworksConfig.h"

OptionsPopup::OptionsPopup(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	inUseKey = nullptr;

	BloodworksConfig* config = bloodworks->getConfig();

	optionsGroup = new cRenderableContainer(bloodworks);
	optionsGroup->setWorldMatrix(Mat3::identity());
	optionsGroup->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	Vec2 size = t->getTexture()->getDimensions().toVec() * 0.65f;
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.65f));
	inside = AARect(size * -0.5f, size * 0.5f);
	optionsGroup->addRenderable(t);
	
	const float titleY = 150.0f;
	const float titleX = 200.0f;

	cTextRenderable* text;
	const float titleButtonSize = 75.0f;

	float x, y;

	// gameplay
	gameplayTitle = new cButton(bloodworks);
	gameplayTitle->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));
	gameplayTitle->setAlignment(RenderableAlignment::center);
	gameplayTitle->setDefaultMatrix(Vec2(-titleX, titleY), Vec2(1.0f), 0.0f);
	gameplayTitle->setHoverMatrix(Vec2(-titleX, titleY), Vec2(1.3f), 0.0f);
	gameplayTitle->setHitArea(Vec2(-titleButtonSize, -20.0f), Vec2(titleButtonSize, 20.0f));
	gameplayTitle->setHoverSpeed(10.0f);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Game-Play", 24.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	gameplayTitle->addRenderable(text);
	optionsGroup->addRenderable(gameplayTitle);

	gameplayGroup = new cRenderableContainer(bloodworks);
	gameplayGroup->setVisible(false);

	float tickSize = 25.0f;
	float fontSize = 23.0f;
	float tickShift = 380.0f;
	float rowShift = 45.0f;
	float sliderShift = 290.0f;

	x = -200.0f;
	y = 40.0f;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Lock Crosshair", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);

	lockCrosshair = new cTickBox(bloodworks);
	lockCrosshair->setWorldMatrix(Mat3::translationMatrix(x, y));
	lockCrosshair->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	lockCrosshair->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	lockCrosshair->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	lockCrosshair->setChecked(config->getLockCrosshair());
	gameplayGroup->addRenderable(lockCrosshair);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Auto Open Perk PopUp", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);

	autoLevelUp = new cTickBox(bloodworks);
	autoLevelUp->setWorldMatrix(Mat3::translationMatrix(x, y));
	autoLevelUp->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	autoLevelUp->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	autoLevelUp->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	autoLevelUp->setChecked(config->getAutoOpenLevelupPopup());
	gameplayGroup->addRenderable(autoLevelUp);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Disable Mods", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);
	disableModsText = text;

	disableMods = new cTickBox(bloodworks);
	disableMods->setWorldMatrix(Mat3::translationMatrix(x, y));
	disableMods->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	disableMods->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	disableMods->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	disableMods->setChecked(config->getAutoOpenLevelupPopup());
	gameplayGroup->addRenderable(disableMods);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Gore", fontSize, Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);
	gore = new cTickBox(bloodworks);
	gore->setWorldMatrix(Mat3::translationMatrix(x, y));
	gore->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	gore->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	gore->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	gameplayGroup->addRenderable(gore);

	gore->setChecked(config->getGore());
	y -= rowShift;
	text->setVisible(false);
	gore->setVisible(false);


	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Screen Shake", fontSize, Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	gameplayGroup->addRenderable(text);

	screenShake = new cTickBox(bloodworks);
	screenShake->setWorldMatrix(Mat3::translationMatrix(x, y));
	screenShake->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	screenShake->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	screenShake->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	screenShake->setChecked(config->getScreenShake());
	gameplayGroup->addRenderable(screenShake);

	y -= rowShift;
	text->setVisible(false);
	screenShake->setVisible(false);


	optionsGroup->addRenderable(gameplayGroup);
	// input

	inputTitle = new cButton(bloodworks);
	inputTitle->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));
	inputTitle->setAlignment(RenderableAlignment::center);
	inputTitle->setDefaultMatrix(Vec2(0.0f, titleY), Vec2(1.0f), 0.0f);
	inputTitle->setHoverMatrix(Vec2(0.0f, titleY), Vec2(1.3f), 0.0f);
	inputTitle->setHitArea(Vec2(-titleButtonSize, -20.0f), Vec2(titleButtonSize, 20.0f));
	inputTitle->setHoverSpeed(10.0f);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Input", 24.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	inputTitle->addRenderable(text);
	optionsGroup->addRenderable(inputTitle);

	inputGroup = new cScrollContainer(bloodworks);
	inputGroup->setAlignment(RenderableAlignment::center);
	inputGroup->setVisible(false);

	x = -240.0f;
	float startY = 80.0f;
	y = startY;
	tickShift = 0.0f;
	sliderShift = 390.0f;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Sensitivity", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	inputGroup->addRenderable(text);

	sensitivity = new cSlider(bloodworks);
	sensitivity->setWorldMatrix(Mat3::translationMatrix(x + sliderShift, y - 5.0f));
	sensitivity->setMinMax(0, 100);
	sensitivity->setValue((int)round(config->getSensitivity() * 100));
	inputGroup->addRenderable(sensitivity);
	y -= rowShift;


	controlSwitch = new cButton(bloodworks);
	controlSwitch->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), nullptr);
	cTexturedQuadRenderable *quad = new cTexturedQuadRenderable(bloodworks, "resources/ui/slider_bg.png", "resources/default");
	Vec2 bgSize = quad->getTexture()->getDimensions().toVec();
	quad->setWorldMatrix(Mat3::scaleMatrix(bgSize));
	controlSwitch->addRenderable(quad);
	controlSwitch->setDefaultMatrix(Vec2(0.0f, y - 5.0f), 1.0f, 0.0f);
	controlSwitch->setHoverMatrix(Vec2(0.0f, y - 5.0f), 1.0f, 0.0f);
	controlSwitch->setHitArea(-bgSize, bgSize);

	controlSwitchText = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "< Keyboard >", 18.0f);
	controlSwitchText->setWorldMatrix(Mat3::translationMatrix(0.0f, bgSize.h * 0.7f));
	controlSwitchText->setTextAlignment(TextAlignment::center);
	controlSwitchText->setVerticalTextAlignment(VerticalTextAlignment::mid);
	controlSwitch->addRenderable(controlSwitchText);
	inputGroup->addRenderable(controlSwitch);

	optionsGroup->addRenderable(inputGroup);

	y -= rowShift;

	float yBase = y;
	for (int i = 0; i < 2; i++)
	{
		y = yBase;
		for (auto& key : BloodworksControls::getKeyData())
		{
			text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), key.keyName, fontSize);
			text->setWorldMatrix(Mat3::translationMatrix(x, y));
			text->setTextAlignment(TextAlignment::left);
			text->setVerticalTextAlignment(VerticalTextAlignment::mid);
			inputGroup->addRenderable(text);

			cKeyMapButton *k = new cKeyMapButton(bloodworks);
			k->setWorldMatrix(Mat3::translationMatrix(x + sliderShift, y - 5.0f));
			inputGroup->addRenderable(k);
			k->setKey(mapper.getMappedKeys(key.key)[i]);
			keyMapButtons.push_back(k);
			if (i > 0)
			{
				k->setVisible(false);
			}
			y -= rowShift * 0.7f;
		}
	}
	inputGroup->setRect(AARect(-300.0f, -180.0f, 300.0f, 110.0f));
	inputGroup->setMaxScroll(startY - y + 20.0f);

	// audio video
	audioVideoTitle = new cButton(bloodworks);
	audioVideoTitle->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));
	audioVideoTitle->setAlignment(RenderableAlignment::center);
	audioVideoTitle->setDefaultMatrix(Vec2(titleX, titleY), Vec2(1.0f), 0.0f);
	audioVideoTitle->setHoverMatrix(Vec2(titleX, titleY), Vec2(1.3f), 0.0f);
	audioVideoTitle->setHitArea(Vec2(-titleButtonSize, -20.0f), Vec2(titleButtonSize, 20.0f));
	audioVideoTitle->setHoverSpeed(10.0f);
	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Audio-Video", 24.0f);
	text->setWorldMatrix(Mat3::identity());
	text->setTextAlignment(TextAlignment::center);
	audioVideoTitle->addRenderable(text);
	optionsGroup->addRenderable(audioVideoTitle);

	audioVideoGroup = new cRenderableContainer(bloodworks);
	audioVideoGroup->setVisible(false);

	x = -200.0f;
	y = 40.0f;
	sliderShift = 290.0f;
	tickShift = 380.0f;

	//text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Resolution", fontSize);
	//text->setWorldMatrix(Mat3::translationMatrix(x, y));
	//text->setTextAlignment(TextAlignment::left);
	//text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	//audioVideoGroup->addRenderable(text);
	//
	//y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Full Screen", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	fullScreen = new cTickBox(bloodworks);
	fullScreen->setWorldMatrix(Mat3::translationMatrix(x, y));
	fullScreen->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	fullScreen->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	fullScreen->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	fullScreen->setChecked(config->getFullScreen());
	audioVideoGroup->addRenderable(fullScreen);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "V-Sync", fontSize, Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	vsync = new cTickBox(bloodworks);
	vsync->setWorldMatrix(Mat3::translationMatrix(x, y));
	vsync->setDefaultMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	vsync->setHoverMatrix(Vec2(x + tickShift, y - 5.0f), Vec2(tickSize), 0.0f);
	vsync->setHitArea(-tickSize * 0.6f, tickSize * 0.6f);
	vsync->setChecked(config->getVSync());
	audioVideoGroup->addRenderable(vsync);

	text->setVisible(false);
	vsync->setVisible(false);

	y -= rowShift * 1.5f;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Volume", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	volume = new cSlider(bloodworks);
	volume->setWorldMatrix(Mat3::translationMatrix(x + sliderShift, y - 5.0f));
	volume->setMinMax(0, 100);
	volume->setValue((int)round(config->getVolume() * 100));
	audioVideoGroup->addRenderable(volume);

	y -= rowShift;

	text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Music", fontSize);
	text->setWorldMatrix(Mat3::translationMatrix(x, y));
	text->setTextAlignment(TextAlignment::left);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	audioVideoGroup->addRenderable(text);

	musicVolume = new cSlider(bloodworks);
	musicVolume->setWorldMatrix(Mat3::translationMatrix(x + sliderShift, y - 5.0f));
	musicVolume->setMinMax(0, 100);
	musicVolume->setValue((int)round(config->getMusicVolume() * 100));
	audioVideoGroup->addRenderable(musicVolume);


	optionsGroup->addRenderable(audioVideoGroup);

	//
	bloodworks->addRenderable(optionsGroup, GUI + 120);

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

	if (bloodworks->isMissionLoaded())
	{
		disableMods->setVisible(false);
		disableModsText->setVisible(false);
	}
	else
	{
		disableMods->setVisible(true);
		disableModsText->setVisible(true);
	}
}

void OptionsPopup::tick()
{
	if (bloodworks->isFirstTick())
	{
		out << "OptionsPopup::tick\n";
	}
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

	BloodworksConfig* config = bloodworks->getConfig();

	if (lastClickedTitle == gameplayTitle)
	{
		gore->check(input.getMousePos());
		screenShake->check(input.getMousePos());
		lockCrosshair->check(input.getMousePos());
		autoLevelUp->check(input.getMousePos());
		disableMods->check(input.getMousePos());

		if (gore->isChanged())
		{
			config->setGore(gore->isChecked());
		}
		if (screenShake->isChanged())
		{
			config->setScreenShake(screenShake->isChecked());
		}
		if (lockCrosshair->isChanged())
		{
			config->setLockCrosshair(lockCrosshair->isChecked());
		}
		if (autoLevelUp->isChanged())
		{
			config->setAutoOpenLevelupPopup(autoLevelUp->isChecked());
		}
		if (disableMods->isChanged())
		{
			config->setModsAreDisabled(disableMods->isChecked());
		}
	}

	if (lastClickedTitle == inputTitle)
	{
		bool isInside = inputGroup->isMouseInside(input.getMousePos());
		Vec2 transformedPos = input.getMousePos() - inputGroup->getScrollMatrix().row2().vec2() / bloodworks->getCameraZoom();

		sensitivity->check(transformedPos, !isInside);
		if (sensitivity->isChanged())
		{
			config->setSensitivity(sensitivity->getIntValue() / 100.0f);
		}

		controlSwitch->check(transformedPos, !isInside);
		if (controlSwitch->isClicked())
		{
			int page = keyMapButtons[0]->isVisible() ? 1 : 0;
			if (page == 0)
			{
				controlSwitchText->setText("< Keyboard >");
			}
			else if (page == 1)
			{
				controlSwitchText->setText("< Joystick >");
			}
			for (int i = 0; i < keyMapButtons.size(); i++)
			{
				auto& key = keyMapButtons[i];
				if (i >= page * (int)GameKey::Count && i < (page + 1)* (int)GameKey::Count)
				{
					key->setVisible(true);
				}
				else
				{
					key->setVisible(false);
				}
			}
		}

		inputGroup->check(input.getMousePos());
		if (inUseKey && inUseKey->isInUse() == false)
		{
			inUseKey->cancel();
			inUseKey = nullptr;
		}
		for (int i=0; i<keyMapButtons.size(); i++)
		{
			auto& key = keyMapButtons[i];
			if (key->isVisible())
			{
				key->check(transformedPos, !isInside);
				if (key->isInUse())
				{
					if (inUseKey != nullptr && inUseKey != key)
					{
						inUseKey->cancel();
					}
					inUseKey = key;
				}
				if (key->isChanged())
				{
					key->mapKeysTo(BloodworksControls::getKeyData()[i].key);
				}
			}
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
			config->setFullScreen(fullScreen->isChecked());
			coral.setFullScreen(fullScreen->isChecked());
			bloodworks->updateZoom();
		}

		if (vsync->isChanged())
		{
			config->setVSync(vsync->isChecked());
		}

		if (volume->isChanged())
		{
			config->setVolume(volume->getIntValue() / 100.0f);
			bloodworks->updateVolume();
		}
		if (musicVolume->isChanged())
		{
			config->setMusicVolume(musicVolume->getIntValue() / 100.0f);
			bloodworks->updateMusicVolume();
		}
	}

	if (input.isKeyPressed(mouse_button_left) && AARect(Vec2(-380, -200), Vec2(380, 200)).isOutside(game->getRelativeMousePos(input.getMousePos(), RenderableAlignment::center)))
	{
		input.clearKeyPress(mouse_button_left);
		optionsGroup->setVisible(false);
	}
	if (mapper.isKeyPressed(GameKey::Back) && inUseKey == nullptr)
	{
		optionsGroup->setVisible(false);
		mapper.clearKeyPress(GameKey::Back);
	}
}

void OptionsPopup::changeTab(cButton *tab, cRenderable *group)
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

	if (inUseKey)
	{
		inUseKey->cancel();
	}
}
