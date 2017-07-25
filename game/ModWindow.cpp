#include "ModWindow.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "cGlobals.h"
#include "cTextRenderable.h"
#include "cButton.h"
#include "cScrollContainer.h"

float buttonHeight = 30.0f;
float height = 35.0f;
float y = 90.0f - buttonHeight * 0.5f;

ModWindow::ModWindow(Bloodworks *bloodworks) 
	: loginWork(this)
	, fetchResults(this)
{
	this->bloodworks = bloodworks;

	mainWindow = new cRenderableContainer(bloodworks);
	mainWindow->setAlignment(RenderableAlignment::center);

	cTexturedQuadRenderable *t = new cTexturedQuadRenderable(bloodworks, "resources/level_up_bg.png", "resources/default");
	t->setWorldMatrix(Mat3::scaleMatrix(t->getTexture()->getDimensions().toVec() * 0.55f));
	mainWindow->addRenderable(t);

	cSlaveController* slaveController = coral.getSlaveController();

	loginning = true;
	fetchingResults = true;

	if (userDetails.hasStoredUser())
	{
		bloodworks->addSlaveWork(&loginWork);
	}

	bloodworks->addSlaveWork(&fetchResults);
	bloodworks->addRenderable(mainWindow, GUI + 151);

	// mod list window
	{
		modListWindow = new cRenderableContainer(bloodworks);
		mainWindow->addRenderable(modListWindow);

		modList = new cScrollContainer(bloodworks);
		modList->setRect(Rect(-300.0f, -150.0f, 275.0f, 90.0f));
		modListWindow->addRenderable(modList);

		//cTexturedQuadRenderable* bg;
		cTextRenderable *text;

		float shift = y + height - 3.0f;
		//bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/mod_name_bg.png", "resources/default");
		//bg->setWorldMatrix(Mat3::scaleMatrix(180.0f, buttonHeight * 0.5f).translateBy(-85.0f, shift));
		//bg->setColor(Vec4::fromColor(0x66FFFFFF));
		//modListWindow->addRenderable(bg);

		text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Mod Name", 18.0f);
		text->setMaxLength(350.0f);
		text->setWorldMatrix(Mat3::translationMatrix(-260.0f, -3.0f + shift));
		text->setMultiline(false);
		modListWindow->addRenderable(text);

		//bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/mod_creator_bg.png", "resources/default");
		//bg->setWorldMatrix(Mat3::scaleMatrix(80.0f, buttonHeight * 0.5f).translateBy(180.0f, shift));
		//bg->setColor(Vec4::fromColor(0x66FFFFFF));
		//modListWindow->addRenderable(bg);

		text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Creator", 18.0f);
		text->setMaxLength(150.0f);
		text->setWorldMatrix(Mat3::translationMatrix(105.0f, -3.0f + shift));
		text->setMultiline(false);
		modListWindow->addRenderable(text);
	}

	setVisible(false);
}

ModWindow::~ModWindow()
{
	SAFE_DELETE(mainWindow);
}

bool ModWindow::isVisible() const
{
	return mainWindow->isVisible();
}

void ModWindow::setVisible(bool visible)
{
	mainWindow->setVisible(visible);
}

void ModWindow::tick()
{
	if (fetchingResults == true && fetchResults.isDone())
	{
		fetchingResults = false;
		for(int i=0; i<5; i++)
		for(int i=0; i<loadedMods.size(); i++)
		{
			auto& modData = loadedMods[i];
			auto& mod = modData.jsonData;
			cButton* button = new cButton(bloodworks);
			button->setHitArea(Vec2(-260.0f, -height * 0.5f), Vec2(250.0f, height * 0.5f));
			button->setDefaultShift(Vec2(0.0f, y + modSelectButtons.size() * -height));
			button->setHoverShift(Vec2(0.0f, y + modSelectButtons.size() * -height));
			button->setHoverScale(1.01f);
			button->setHoverSpeed(10.0f);
			button->setUserData(reinterpret_cast<void*>((size_t)i));
			bool installed = installedModIndices.count(mod["name"].get<std::string>()) > 0;

			cTexturedQuadRenderable* bg;
			cTextRenderable *text;

			bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/mod_name_bg.png", "resources/default");
			bg->setWorldMatrix(Mat3::scaleMatrix(180.0f, buttonHeight * 0.5f).translateBy(-85.0f, 0.0f));
			bg->setColor(Vec4::fromColor(0x66FFFFFF));
			button->addRenderable(bg);

			if (installed)
			{
				bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/installed_mod.png", "resources/default");
				bg->setWorldMatrix(Mat3::scaleMatrix(10.0f).translateBy(-250.0f, 0.0f));
				bg->setColor(Vec4::fromColor(0xFF44FF44));
				button->addRenderable(bg);
			}

			text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), mod["name"].get<std::string>(), 18.0f);
			text->setMaxLength(350.0f - (installed ? 22.0f : 0.0f));
			text->setWorldMatrix(Mat3::translationMatrix(-260.0f + (installed ? 22.0f : 0.0f), -3.0f));
			text->setMultiline(false);
			button->addRenderable(text);

			bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/mod_creator_bg.png", "resources/default");
			bg->setWorldMatrix(Mat3::scaleMatrix(80.0f, buttonHeight * 0.5f).translateBy(180.0f, 0.0f));
			bg->setColor(Vec4::fromColor(0x66FFFFFF));
			button->addRenderable(bg);

			text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), mod["creator"].get<std::string>(), 18.0f);
			text->setMaxLength(150.0f);
			text->setWorldMatrix(Mat3::translationMatrix(105.0f, -3.0f));
			text->setMultiline(false);
			button->addRenderable(text);

			modList->addRenderable(button);
			modSelectButtons.push_back(button);
		}
		modList->setMaxScroll(modSelectButtons.size() * height);
	}

	if (!isVisible())
	{
		return;
	}

	if (modListWindow->isVisible())
	{
		modList->check(input.getMousePos());
		bool isInside = modList->isMouseInside(input.getMousePos());
		Vec2 transformedPos = input.getMousePos() - modList->getScrollMatrix().row2.vec2;

		for (int i = 0; i < modSelectButtons.size(); i++)
		{
			modSelectButtons[i]->check(transformedPos, !isInside);
			if (modSelectButtons[i]->isClicked())
			{
				int modIndex = (int)reinterpret_cast<size_t>(modSelectButtons[i]->getUserData());
				printf("%d\n", modIndex);
			}
		}
	}
}

void ModWindow::addInstalledMod(nlohmann::json& j, DirentHelper::File& f)
{
	ModData m;
	m.filePath = f;
	m.jsonData = j;
	installedMods.push_back(m);
	installedModIndices[m.jsonData["name"].get<std::string>()] = (int)installedMods.size() - 1;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	std::string* str = (std::string*)userp;
	size_t realsize = size * nmemb;
	(*str).append((char*)contents, realsize);
	return realsize;
}

void ModWindow::FetchResults::runOnSlave()
{
	std::string readBuffer = "";
	CURL *curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "http://bloodworks.enginmercan.com/listmods.php");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
	modWindow->loadedMods.clear();
	if (readBuffer.size() > 0)
	{
		nlohmann::json modsJson = nlohmann::json::parse(readBuffer.c_str());;
		if (modsJson.count("mods"))
		{
			for (auto& jsonData : modsJson["mods"])
			{
				ModWindow::ModData modData;
				modData.jsonData = jsonData;
				modWindow->loadedMods.push_back(modData);
			}
		}
	}
}

void ModWindow::FetchResults::runOnMain()
{
}
