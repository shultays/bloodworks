#include "ModWindow.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "Bloodworks.h"
#include "cTexture.h"
#include "cGlobals.h"
#include "cTextRenderable.h"
#include "cButton.h"
#include "cScrollContainer.h"
#include "BloodworksControls.h"

float buttonHeight = 30.0f;
float height = 35.0f;
float y = 90.0f - buttonHeight * 0.5f;

void ModWindow::showModDetails(struct ModData& modData)
{
	modListWindow->setVisible(false);
	modDetailWindow->setVisible(true);

	detailName->setText(modData.jsonData["name"].get<std::string>());
	detailVersion->setText("Version: " + modData.jsonData["version"].get<std::string>());
	detailCreator->setText("Creator: " + modData.jsonData["creator"].get<std::string>());
	detailDescription->setText(modData.jsonData["description"].get<std::string>());

	detailedMod = modData;

	std::stringstream ss;
	ss << TEMP_FOLDER "temp_icon_";
	ss << modData.jsonData["id"].get<int>();
	ss << ".png";

	if (std::ifstream(ss.str()))
	{
		detailIcon->setTexture(ss.str());
	}
	else
	{
		detailIcon->setTexture("");
		fetchImage.id = modData.jsonData["id"].get<int>();
		fetchImage.modWindow = this;
		fetchingImageId = fetchImage.id;
		bloodworks->addSlaveWork(&fetchImage);
	}


	detailInstalled = installedModIndices.count(modData.jsonData["name"].get<std::string>()) > 0;

	if (detailInstalled)
	{
		detailInstallButton->setVisible(false);
		detailInstallText->setVisible(true);
		detailInstallText->setText("Installed");
	}
	else
	{
		detailInstallButton->setVisible(true);
		detailInstallText->setVisible(false);
		detailInstallText->setText("");
	}
}

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
		modList->setRect(AARect(-300.0f, -150.0f, 275.0f, 90.0f));
		modListWindow->addRenderable(modList);

		cTextRenderable *text;
		float shift = y + height - 3.0f;
		text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Mod Name", 18.0f);
		text->setMaxLength(350.0f);
		text->setMaxLineCount(1);
		text->setWorldMatrix(Mat3::translationMatrix(-260.0f, -3.0f + shift));
		modListWindow->addRenderable(text);

		text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Creator", 18.0f);
		text->setMaxLength(150.0f);
		text->setMaxLineCount(1);
		text->setWorldMatrix(Mat3::translationMatrix(105.0f, -3.0f + shift));
		modListWindow->addRenderable(text);
	}

	{
		modDetailWindow = new cRenderableContainer(bloodworks);
		mainWindow->addRenderable(modDetailWindow);

		detailName = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 24.0f);
		detailName->setMaxLength(550.0f);
		detailName->setVerticalTextAlignment(VerticalTextAlignment::mid);
		detailName->setWorldMatrix(Mat3::translationMatrix(Vec2(10.0f, 130.0f)));
		detailName->setTextAlignment(TextAlignment::center);
		modDetailWindow->addRenderable(detailName);

		detailIcon = new cTexturedQuadRenderable(bloodworks, "", "resources/default");
		detailIcon->setWorldMatrix(Mat3::scaleMatrix(60.0).translateBy(190.0f, 35.0f));
		modDetailWindow->addRenderable(detailIcon);

		detailVersion = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 14.0f);
		detailVersion->setWorldMatrix(Mat3::translationMatrix(Vec2(190.0f, -40.0f)));
		detailVersion->setTextAlignment(TextAlignment::center);
		modDetailWindow->addRenderable(detailVersion);

		detailCreator = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 14.0f);
		detailCreator->setMaxLength(170.0f);
		detailCreator->setWorldMatrix(Mat3::translationMatrix(Vec2(190.0f, -60.0f)));
		detailCreator->setTextAlignment(TextAlignment::center);
		modDetailWindow->addRenderable(detailCreator);

		detailDescription = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 15.0f);
		detailDescription->setMaxLength(350.0f);
		detailDescription->setWorldMatrix(Mat3::translationMatrix(Vec2(-265.0f, 80.0f)));
		detailDescription->setTextAlignment(TextAlignment::fit);
		detailDescription->setMaxLineCount(12);
		modDetailWindow->addRenderable(detailDescription);

		detailInstallText = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "", 18.0f);
		detailInstallText->setWorldMatrix(Mat3::translationMatrix(190.0f, -140.0f));
		detailInstallText->setTextAlignment(TextAlignment::center);
		modDetailWindow->addRenderable(detailInstallText);


		detailInstallButton = new cButton(bloodworks);
		detailInstallButton->setHitArea(Vec2(-260.0f, -height * 0.5f), Vec2(250.0f, height * 0.5f));
		detailInstallButton->setDefaultShift(Vec2(190.0f, -140.0f));
		detailInstallButton->setHoverShift(Vec2(190.0f, -140.0f));
		detailInstallButton->setHoverScale(1.1f);
		detailInstallButton->setHoverSpeed(10.0f);

		cTexturedQuadRenderable *bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/install_bg.png", "resources/default");
		bg->setWorldMatrix(Mat3::scaleMatrix(80.0f, buttonHeight * 0.5f));
		bg->setColor(Vec4::fromColor(0x66FFFFFF));
		detailInstallButton->addRenderable(bg);

		cTextRenderable *text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), "Install", 18.0f);
		text->setWorldMatrix(Mat3::translationMatrix(-3.0f, -3.0f));
		text->setTextAlignment(TextAlignment::center);
		detailInstallButton->addRenderable(text);

		modDetailWindow->addRenderable(detailInstallButton);
	}

	modDetailWindow->setVisible(false);
	modListWindow->setVisible(false);

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
	if (visible)
	{
		modListWindow->setVisible(true);
		modDetailWindow->setVisible(false);
	}
}

void ModWindow::tick()
{

	if (!isVisible())
	{
		return;
	}

	if (fetchingResults == true && fetchResults.isDone())
	{
		fetchingResults = false;
		updateList();
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
				showModDetails(loadedMods[modIndex]);
			}
		}	
		if (mapper.isKeyPressed(GameKey::Back))
		{
			setVisible(false);
		}
	}
	else if (modDetailWindow->isVisible())
	{
		detailInstallButton->check(input.getMousePos());
		if (detailInstallButton->isClicked())
		{
			fetchMod.modWindow = this;
			fetchMod.id = detailedMod.jsonData["id"].get<int>();
			fetchMod.byteToLoad = 1;
			fetchMod.readBuffer = "";
			fetchMod.valid = false;
			bloodworks->addSlaveWork(&fetchMod);
			detailInstallButton->setVisible(false);
			detailInstallText->setVisible(true);
		}
		if (detailInstallText->isVisible())
		{
			if (detailInstalled == false)
			{
				if (fetchMod.isDone())
				{
					detailInstallText->setText("Installed");
					if (fetchMod.valid)
					{
						fetchMod.valid = false;
						bloodworks->loadMod(fetchMod.installPath);
						updateList();
						detailInstalled = true;
					}
				}
				else
				{
					std::stringstream ss;
					ss << round((fetchMod.readBuffer.size() * 100.0f) / fetchMod.byteToLoad);
					ss << "%";
					detailInstallText->setText(ss.str());
				}
			}
		}
		if (mapper.isKeyPressed(GameKey::Back))
		{
			detailIcon->setTexture(""); // todo bug? try removing does not clear the texture
			setVisible(true);
		}
	}
}

void ModWindow::addInstalledMod(nlohmann::json& j, DirentHelper::File& f)
{
	ModData m;
	m.filePath = f;
	m.jsonData = j;
	installedMods.push_back(m);
	installedModIndices[m.jsonData["name"].get<std::string>()] = installedMods.size() - 1;
}

void ModWindow::updateList()
{
	for (int i = 0; i < modSelectButtons.size(); i++)
	{
		modList->removeRenderable(modSelectButtons[i]);
		SAFE_DELETE(modSelectButtons[i]);
	}
	modSelectButtons.clear();
	for (int i = 0; i < loadedMods.size(); i++)
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
		text->setMaxLineCount(1);
		button->addRenderable(text);

		bg = new cTexturedQuadRenderable(bloodworks, "resources/ui/mod_creator_bg.png", "resources/default");
		bg->setWorldMatrix(Mat3::scaleMatrix(80.0f, buttonHeight * 0.5f).translateBy(180.0f, 0.0f));
		bg->setColor(Vec4::fromColor(0x66FFFFFF));
		button->addRenderable(bg);

		text = new cTextRenderable(bloodworks, resources.getFont("resources/fontData.txt"), mod["creator"].get<std::string>(), 18.0f);
		text->setMaxLength(150.0f);
		text->setWorldMatrix(Mat3::translationMatrix(105.0f, -3.0f));
		text->setMaxLineCount(1);
		button->addRenderable(text);

		modList->addRenderable(button);
		modSelectButtons.push_back(button);
	}
	modList->setScroll(0.0f);
	modList->setMaxScroll(modSelectButtons.size() * height);
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
		nlohmann::json modsJson = nlohmann::json::parse(readBuffer.c_str());
		readBuffer = "";
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

void ModWindow::FetchImage::runOnSlave()
{
	std::string readBuffer = "";
	int id = this->id;
	CURL *curl = curl_easy_init();
	if (curl)
	{
		std::stringstream ss;
		ss << "http://bloodworks.enginmercan.com/downloadicon.php?id=";
		ss << id;

		curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_HTTPGET, WriteCallback);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			std::stringstream ss;
			ss << TEMP_FOLDER "temp_icon_";
			ss << id;
			ss << ".png";

			std::ofstream out(ss.str(), std::ios::binary);
			out << readBuffer;
			out.close();
		}
		curl_easy_cleanup(curl);
	}
}

void ModWindow::FetchImage::runOnMain()
{
	if (id == modWindow->fetchingImageId)
	{
		std::stringstream ss;
		ss << TEMP_FOLDER "temp_icon_";
		ss << id;
		ss << ".png";
		modWindow->detailIcon->setTexture(ss.str());
	}
}

size_t HeaderCallBack(char *buffer, size_t size, size_t nitems, void *userdata)
{
	std::string s(buffer, nitems);
	std::string toFind = "Content-length: ";
	size_t pos = s.find(toFind);
	if (pos != std::string::npos)
	{
		s = s.substr(pos + toFind.size());
		for (int i = 0; i < s.size(); i++)
		{
			if (s[i] < '0' || s[i] > '9')
			{
				s[i] = '\0';
				break;
			}
		}
		int totalSize = atoi(s.c_str());
		reinterpret_cast<ModWindow::FetchMod*>(userdata)->byteToLoad = totalSize;
	}
	return nitems * size;
}


void ModWindow::FetchMod::runOnSlave()
{
	CURL *curl = curl_easy_init();
	if (curl)
	{
		std::stringstream ss;
		ss << "http://bloodworks.enginmercan.com/download.php?id=";
		ss << id;
		curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

		curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallBack);

		curl_easy_setopt(curl, CURLOPT_HTTPGET, WriteCallback);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else
		{
			std::stringstream ss;
			ss << TEMP_FOLDER "temp_mod_";
			ss << id;
			ss << ".bld";

			std::ofstream out(ss.str(), std::ios::binary);
			out << readBuffer;
			out.close();
			valid = true;
		}
		curl_easy_cleanup(curl);
	}
}

void ModWindow::FetchMod::runOnMain()
{
	if (valid)
	{
		std::stringstream ss;
		ss << TEMP_FOLDER "temp_mod_";
		ss << id;
		ss << ".bld";

		std::string mod_folder;
		if (modWindow->detailedMod.jsonData.count("folder"))
		{
			mod_folder = modWindow->detailedMod.jsonData["folder"].get<std::string>();
		}
		else
		{
			std::stringstream ss;
			ss << "mod_";
			ss << id;
			mod_folder = ss.str();
		}
		bool addedUnderScore = false;
		std::string folder = "resources/mods/" + mod_folder;
		while (true)
		{
			DirentHelper::Folder f(folder);
			if (f.exists() == false)
			{
				folder += "_copy";
			}
			else
			{
				break;
			}
		}
		fixFolderPath(folder);
		installPath = folder;
		cPackHelper::unpackFile(ss.str(), folder);
	}
}
