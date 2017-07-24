#include "ModWindow.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "cGlobals.h"
#include "cTextRenderable.h"
#include "cButton.h"
#include "cScrollContainer.h"

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

	modList = new cScrollContainer(bloodworks);
	modList->setRect(Rect(-300.0f, -150.0f, 275.0f, 90.0f));
	mainWindow->addRenderable(modList);

	cSlaveController* slaveController = coral.getSlaveController();

	loginning = true;
	fetchingResults = true;

	if (userDetails.hasStoredUser())
	{
		bloodworks->addSlaveWork(&loginWork);
	}

	bloodworks->addSlaveWork(&fetchResults);
	bloodworks->addRenderable(mainWindow, GUI + 151);

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

		float buttonHeight = 30.0f;
		float height = 35.0f;
		float y = 90.0f - buttonHeight * 0.5f;
		if (modsJson.count("mods"))
		{
			for (int i = 0; i < 10; i++)
			{
				for (auto& mod : modsJson["mods"])
				{
					cButton* button = new cButton(bloodworks);
					button->setHitArea(Vec2(-260.0f, -height * 0.5f), Vec2(250.0f, height * 0.5f));
					button->setDefaultShift(Vec2(0.0f, y + modSelectButtons.size() * -height));
					button->setHoverShift(Vec2(0.0f, y + modSelectButtons.size() * -height));
					button->setHoverScale(1.01f);
					button->setHoverSpeed(10.0f);

					cTexturedQuadRenderable* bg;
					cTextRenderable *text;

					bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/mod_name_bg.png", "resources/default");
					bg->setWorldMatrix(Mat3::scaleMatrix(180.0f, buttonHeight * 0.5f).translateBy(-85.0f, 0.0f));
					bg->setColor(Vec4::fromColor(0x66FFFFFF));
					button->addRenderable(bg);

					text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), mod["name"].get<std::string>(), 18.0f);
					text->setMaxLength(350.0f);
					text->setWorldMatrix(Mat3::translationMatrix(-260.0f, -3.0f));
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
			}
			modList->setMaxScroll(modSelectButtons.size() * height);
		}
	}

	if (!isVisible())
	{
		return;
	}

	if (modList->isVisible())
	{
		modList->check(input.getMousePos());
		bool isInside = modList->isMouseInside(input.getMousePos());
		Vec2 transformedPos = input.getMousePos() - modList->getScrollMatrix().row2.vec2;

		for (int i = 0; i < modSelectButtons.size(); i++)
		{
			modSelectButtons[i]->check(transformedPos, !isInside);
			if (modSelectButtons[i]->isClicked())
			{
				printf("%d\n", i);
			}
		}
	}
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
	if (readBuffer.size() > 0)
	{
		modWindow->modsJson = nlohmann::json::parse(readBuffer.c_str());
	}
}

void ModWindow::FetchResults::runOnMain()
{
}
