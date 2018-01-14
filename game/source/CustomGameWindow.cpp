#include "CustomGameWindow.h"
#include "Bloodworks.h"
#include "cRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "cTextRenderable.h"
#include "cFont.h"
#include "cTexture.h"
#include "cButton.h"
#include "Bloodworks.h"
#include "cScrollContainer.h"
#include "BloodworksControls.h"

CustomGameWindow::CustomGameWindow(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	window = new cRenderableContainer(bloodworks);
	window->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.55f));
	window->addRenderable(t);

	title = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 24.0f);
	title->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, 110.0f)));
	title->setTextAlignment(TextAlignment::center);
	title->setMaxLength(550.0f);
	title->setMaxLineCount(1);
	window->addRenderable(title);

	nomods = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "No custom game is available.\n\nYou can download custom games using \"Manage Mods\" window.", 18.0f, Vec4::fromColor(0xFFCCCCCC));
	nomods->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, -10.0f)));
	nomods->setTextAlignment(TextAlignment::center);
	nomods->setVerticalTextAlignment(VerticalTextAlignment::mid);
	nomods->setMaxLength(450.0f);
	window->addRenderable(nomods);

	currentDescription = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 14.0f);
	currentDescription->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, -130.0f)));
	currentDescription->setTextAlignment(TextAlignment::center);
	currentDescription->setVerticalTextAlignment(VerticalTextAlignment::mid);
	currentDescription->setMaxLength(500.0f);
	currentDescription->setMaxLineCount(3);
	window->addRenderable(currentDescription);

	buttonContainer = new cScrollContainer(bloodworks);
	buttonContainer->setRect(AARect(-300.0f, -100.0f, 275.0f, 90.0f));
	window->addRenderable(buttonContainer);

	bloodworks->addRenderable(window, GUI + 150);

	window->setVisible(false);
}

CustomGameWindow::~CustomGameWindow()
{
	clearButtons();
	SAFE_DELETE(window);
}

void CustomGameWindow::clearButtons()
{
	for (auto b : customGameButtons)
	{
		buttonContainer->removeRenderable(b);
		SAFE_DELETE(b);
	}
	customGameButtons.clear();
}

bool CustomGameWindow::isVisible() const
{
	return window->isVisible();
}

void CustomGameWindow::show()
{
	window->setVisible(true);

	folder.clear();
	currentMods.clear();

	loadMods();

	nomods->setVisible(customGameButtons.size() == 0);
}

void CustomGameWindow::tick()
{
	if (bloodworks->isFirstTick())
	{
		out << "CustomGameWindow::tick\n";
	}
	if (isVisible() == false)
	{
		return;
	}
	bool descriptionSet = false;
	buttonContainer->check(input.getMousePos());
	bool isInside = buttonContainer->isMouseInside(input.getMousePos());

	for (int i=0; i<customGameButtons.size(); i++)
	{
		cButton* button = customGameButtons[i];
		button->check(input.getMousePos(), !isInside);

		if (button->isHovering())
		{
			title->setText(currentMods[i].name);
			currentDescription->setText(currentMods[i].description);
			descriptionSet = true;
		}

		if (button->isClicked())
		{
			if (currentMods[i].folder)
			{
				folder.push_back(currentMods[i].folderPath);
				loadMods();
			}
			else
			{
				window->setVisible(false);
				bloodworks->loadMission(currentMods[i].scriptName);
			}
		}
	}
	if (descriptionSet == false)
	{
		title->setText("Custom Games");
		currentDescription->setText("");
	}

	if (mapper.isKeyPressed(GameKey::Back) || (input.isKeyPressed(mouse_button_left) && AARect(Vec2(-380, -200), Vec2(380, 200)).isOutside(game->getRelativeMousePos(input.getMousePos(), RenderableAlignment::center))))
	{
		if (folder.size() > 0)
		{
			folder.resize(folder.size() - 1);
			loadMods();
		}
		else
		{
			window->setVisible(false);
		}
	}
}

void CustomGameWindow::loadMods()
{
	clearButtons();
	MissionController *missionController = bloodworks->getMissionController();

	missionController->getMissions(currentMods, folder);

	float distance = 100.0f;
	int modCount = currentMods.size();
	int lastRow = modCount / 5;

	float rowSize = 150.0f;
	int current = 0;
	for (int i = 0; i < currentMods.size(); i++)
	{
		auto& mod = currentMods[i];

		int page = current / 5;
		int rowCount = 5;
		if (page == lastRow)
		{
			rowCount = modCount % 5;
		}

		int row = current % 5;

		cButton *t = new cButton(bloodworks);
		Vec2 pos = Vec2(-row * distance + (rowCount - 1) * distance * 0.5f, 40.0f -page * rowSize);
		t->setDefaultMatrix(pos, 1.0f, 0.0f);
		t->setHoverMatrix(pos, 1.1f, 0.0f);
		t->setAlignment(RenderableAlignment::center);
		t->setColor(Vec4(1.0f, 1.0f, 1.0f, 0.0f));
		t->setHitArea(Vec2(-50.0f), Vec2(50.0f));
		t->setHoverSpeed(10.0f);
		t->setSounds(resources.getSoundSample("resources/sounds/click.ogg"), resources.getSoundSample("resources/sounds/hover.ogg"));

		cTexturedQuadRenderable *quad = new cTexturedQuadRenderable(bloodworks, mod.icon == "" ? mod.folder ? "resources/missions/default_folder_icon.png" : "resources/missions/default_icon.png" : mod.icon, "resources/default");
		quad->setWorldMatrix(Mat3::scaleMatrix(40.0f));
		t->addRenderable(quad);

		cTextRenderable *nameRenderable = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), mod.name, 14.0f);
		nameRenderable->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, -50.0f)));
		nameRenderable->setTextAlignment(TextAlignment::center);
		nameRenderable->setVerticalTextAlignment(VerticalTextAlignment::bottom);
		nameRenderable->setMaxLength(90.0f);
		nameRenderable->setMaxLineCount(3);
		t->addRenderable(nameRenderable);


		buttonContainer->addRenderable(t);
		customGameButtons.push_back(t);

		current++;
	}
	buttonContainer->setMaxScroll((((modCount -1 )/ 5) + 1) * rowSize + 20.0f);
}
