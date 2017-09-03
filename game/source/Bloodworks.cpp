#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Bullet.h"
#include "Monster.h"
#include "Gun.h"
#include "Bonus.h"
#include "BloodRenderable.h"
#include "cFont.h"
#include "Perk.h"
#include "Bullet.h"
#include "cParticle.h"
#include "cPostProcess.h"
#include "LevelUpPopup.h"
#include "ExplosionController.h"
#include "DropController.h"
#include "MonsterController.h"
#include "BulletController.h"
#include "MissionController.h"
#include "BloodworksLuaWorld.h"
#include "DirentHelper.h"
#include "LaserRenderable.h"
#include "MainMenu.h"
#include "cSoundManager.h"
#include "OneShotSoundManager.h"
#include "BuffFloat.h"
#include "OptionsPopup.h"
#include "cPersistent.h"
#include "BloodworksControls.h"
#include "cTexturedQuadRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "ModWindow.h"
#include "cSlave.h"
#include "BloodworksConfig.h"
#include "BloodworksCheats.h"
#include "cTimeProfiler.h"
#include "cAnimationTemplate.h"
#include "GroundRenderable.h"
#include "CollisionController.h"
#include "GameObjectTemplate.h"
#include "CrashReporterWindow.h"
#include "CreditsWindow.h"
#include <sstream>

#ifdef HAS_BLOODWORKS_CHEATS
BloodworksCheats *bloodworksCheats;
#endif

void appendJson(nlohmann::json& j, const std::string& fileName)
{
	out << "json " << fileName << "\n";
	std::string jsonFile2;
	textFileRead(fileName, jsonFile2);
	nlohmann::json j2 = nlohmann::json::parse(jsonFile2.c_str());
	if (j2.count("baseFile"))
	{
		appendJson(j, j2["baseFile"].get<std::string>());
	}

	for (nlohmann::json::iterator it = j2.begin(); it != j2.end(); ++it)
	{
		j[it.key()] = it.value();
	}
}

void Bloodworks::initImplementation()
{
	nextGlobalUniqueId = 1;

	lua.script(R"(
			doNotDelete = {}
			for k in pairs (_G) do
				doNotDelete[k] = true
			end
		)");

	if (config == nullptr)
	{
		config = new BloodworksConfig();
	}

	coral.setWindowSize(config->getWindowWidth(), config->getWindowHeight());

	luaWorld = new BloodworksLuaWorld(this);
	luaWorld->reset();

	lua.script_file("resources/helpers.lua");
	lua.script_file("resources/missions/helpers.lua");
	lua.script_file("resources/guns/helpers.lua");
	lua.script_file("resources/monsters/helpers.lua");

	coral.getSoundManager()->setGlobalVolume(config->getVolume());

	lua["time"] = timer.getTime();

	dropController = new DropController(this);
	monsterController = new MonsterController(this);
	bulletController = new BulletController(this);
	collisionController = new CollisionController(this);
	missionController = new MissionController(this);
	oneShotSoundManager = new OneShotSoundManager(this);
	modWindow = new ModWindow(this);
	creditsWindow = new CreditsWindow(this);

	player = new Player(this);

	loadMod("resources");

	bg = new GroundRenderable(this);
	addRenderable(bg, BACKGROUND);

	cShaderShr shader = resources.getShader("resources/defaultWithUVScale.vs", "resources/default.ps");
	int uvBegin = shader->addUniform("uvBegin", TypeVec2).index;
	int uvSize = shader->addUniform("uvSize", TypeVec2).index;

	const Vec2& mapBegin = mapRect.getMin();
	const Vec2& mapEnd = mapRect.getMax();

	cTextureShr fgBlack = resources.getTexture("resources/fg_black.png", true);
	cTexturedQuadRenderable *fg;
	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, mapSize.y * 0.5f - 40.0f).translateBy(mapBegin.x, 0.0f));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.0f, 0.5f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, mapSize.y * 0.5f - 40.0f).translateBy(mapEnd.x, 0.0f));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.8f, 0.5f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(mapSize.x * 0.5f - 40.0f, 40.0f).translateBy(0.0f, mapBegin.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.5f, 0.8f));
	fg->setUniform(uvSize, Vec2(0.0f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(mapSize.x * 0.5f - 40.0f, 40.0f).translateBy(0.0f, mapEnd.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.5f, 0.0f));
	fg->setUniform(uvSize, Vec2(0.0f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapBegin.x, mapBegin.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.0f, 0.8f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapBegin.x, mapEnd.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.0f, 0.0f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapEnd.x, mapBegin.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.8f, 0.8f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, fgBlack, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapEnd.x, mapEnd.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.8f, 0.0f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	cTextureShr black = resources.getTexture("resources/black.png", true);

	fg = new cTexturedQuadRenderable(this, black, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f, 600.0f + mapSize.y).translateBy(mapBegin.x - 640.0f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	fg = new cTexturedQuadRenderable(this, black, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f, 600.0f + mapSize.y).translateBy(mapEnd.x + 640.0f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, black, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f + mapSize.x, 600.0f).translateBy(0.0f, mapBegin.y - 640.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	fg = new cTexturedQuadRenderable(this, black, "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f + mapSize.x, 600.0f).translateBy(0.0f, mapEnd.y + 640.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	input.hideMouse();

	player->setGun(guns[0]);

	bloodRenderable = new BloodRenderable(this);
	bloodRenderable->init();
	addRenderable(bloodRenderable, BACKGROUND + 1);

	explosionController = new ExplosionController(this);

	pausePostProcess = new cPostProcess();
	pausePostProcess->init(this, resources.getShader("resources/post_process/default.vs", "resources/post_process/pause.ps"), 200);
	pausePostProcess->setShaderWeight(0.0f);
	pausePostProcess->setEnabled(false);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	mainMenu = new MainMenu(this);

	levelUpPopup = new LevelUpPopup(this);
	optionsPopup = new OptionsPopup(this);

	coral.setFullScreen(config->getFullScreen());
	mainMenu->setVisible(true);

#ifdef HAS_BLOODWORKS_CHEATS
	bloodworksCheats = new BloodworksCheats(this);
	bloodworksCheats->onInit();
#endif
}

void Bloodworks::init()
{
	if (Coral::isDebuggerPresent() == false)
	{
		std::ifstream f(STD_OUTPUT_COPY);

		if (f.good())
		{
			config = new BloodworksConfig();

			if (config->getCrashAutoSendState() == 0)
			{
				crashReporterWindow = new CrashReportWindow(this);
				return;
			}
			else
			{
				if (config->getCrashAutoSendState() == 1)
				{
					SendReport("in-game report", true);
				}
				f.close();
				cPackHelper::deleteFile(STD_OUTPUT_COPY);
			}
		}
	}
	initImplementation();

	static bool noSleep = getConfig()->getBool("no_sleep", false, "Disables CPU going idle, set to 1 if there are performance problems (will make your CPU scream in pain)");
	if (noSleep == true)
	{
		coral.setNoSleep(noSleep);
	}
}

Bloodworks::Bloodworks()
{
	nextUniqueId = 1000;
	nextGlobalUniqueId = 1;

	mapSize = 2000.0f;
	mapRect.set(-mapSize * 0.5f, +mapSize * 0.5f);

	postProcessEndLevel = (GUI + FOREGROUND) / 2;

	pauseSlowdown = 1.0f;
	paused = false;

	soundPaused = false;

	config = nullptr;
	crashReporterWindow = nullptr;

	BloodworksControls::init();
}

Bloodworks::~Bloodworks()
{
	clear();
}

void Bloodworks::onAddedGunBullet(Gun *gun, Bullet *bullet)
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			bonus->onAddGunBullet(gun, bullet);
		}
	}
	for (auto& perk : usedPerks)
	{
		perk->onAddGunBullet(gun, bullet);
	}
}

void Bloodworks::doPause()
{
	paused = true;
	pausePostProcess->setEnabled(true);
}

void Bloodworks::doUnpause()
{
	paused = false;
}

cVector<Perk*> Bloodworks::getAvailablePerks() const
{
	cVector<Perk*> availablePerks;
	for (auto& perk : perks)
	{
		if (perk->isTakenFully() == false)
		{
			availablePerks.push_back(perk);
		}
	}
	return availablePerks;
}

void Bloodworks::onPerkUsed(Perk *levelupPerks)
{
	if (levelupPerks->getLevel() == 1)
	{
		usedPerks.push_back(levelupPerks);
	}
}

int Bloodworks::onPlayerDamaged(int damage, float dir, sol::table& params)
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			damage = bonus->onPlayerDamaged(damage, dir, params);
		}
	}
	for (auto& perk : usedPerks)
	{
		damage = perk->onPlayerDamaged(damage, dir, params);
	}
	if (player->getGun())
	{
		damage = player->getGun()->onPlayerDamaged(damage, dir, params);
	}
	if (player->getSecondaryGun())
	{
		damage = player->getSecondaryGun()->onPlayerDamaged(damage, dir, params);
	}
	return damage;
}

void Bloodworks::addLaserTemplate(LaserTemplate *laserTemplate)
{
	laserTemplates[laserTemplate->getName()] = laserTemplate;
}

bool Bloodworks::isLevelUpPopupVisible() const
{
	return levelUpPopup->isVisible();
}

void Bloodworks::windowResized(int width, int height)
{
	cGame::windowResized(width, height);
	mainMenu->resize();
	player->resize();
	missionController->repositionGUI();
	if (coral.isFullScreen() == false && coral.isDebuggerPresent() == false)
	{
		config->setWindowWidth(width);
		config->setWindowHeight(height);
	}
}

void Bloodworks::clearMission()
{
	lua["gameObjects"] = lua.create_table();
	lua["mission"] = lua.create_table();

	coral.getSoundManager()->clearAllSounds();

	setSoundSpeed(1.0f);
	setSlowdown(1.0f);

	missionController->reset();
	collisionController->reset();
	player->reset();
	monsterController->reset();
	bloodRenderable->reset();
	dropController->reset();
	bulletController->reset();
	explosionController->reset();
	levelUpPopup->reset();
	luaWorld->reset();

	cameraCenterPos.setZero();
	setCameraPos(cameraCenterPos);

	cVector<cPostProcess*> toRemove;

	for (int i = 0; i<postProcesses.size(); i++)
	{
		if (postProcesses[i] != pausePostProcess)
		{
			toRemove.push_back(postProcesses[i]);
		}
	}
	for (auto& postProcess : toRemove)
	{
		SAFE_DELETE(postProcess);
	}
	toRemove.clear();

	for (auto& particle : orphanParticles)
	{
		SAFE_DELETE(particle);
	}
	orphanParticles.clear();

	for (auto& perk : perks)
	{
		perk->reset();
	}
	usedPerks.clear();
	for (auto& bonus : bonuses)
	{
		bonus->reset();
	}
	activeBonuses.clear();
	for (auto& gun : guns)
	{
		gun->reset();
	}
	cameraPos.setZero();
	input.showMouse();
}

bool Bloodworks::gotoMainMenu()
{
	if (levelUpPopup->isVisible())
	{
		return false;
	}
	clearMission();
	setMainMenuVisible();
	return true;
}

bool Bloodworks::loadMission(const std::string& mission)
{
	if (levelUpPopup->isVisible())
	{
		return false;
	}
	clearMission();

	mainMenu->setVisible(false);
	input.hideMouse();

	for (auto& gun : guns)
	{
		if (gun->getScriptName() == "Pistol")
		{
			player->setGun(gun);
		}
	}
	missionController->loadMission(mission);
	player->setVisible(true);
#ifdef HAS_BLOODWORKS_CHEATS
	bloodworksCheats->onLoadMission();
#endif 

	return true;
}

void Bloodworks::onPlayerDied()
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			bonus->onPlayerDied();
		}
	}
	for (auto& perk : usedPerks)
	{
		perk->onPlayerDied();
	}
	missionController->onPlayerDied();
}

void Bloodworks::playSoundAtMap(const Vec2& pos, cSoundSampleShr s, float volume)
{
	float volumeMultiplier = getVolumeMultiplier(pos);

	if (volumeMultiplier > 0.0f)
	{
		addGameSound(s->play(volumeMultiplier * volume));
	}
}

void Bloodworks::playSoundAtMap(const Vec2& pos, cSoundSampleWithParams s)
{
	float volumeMultiplier = getVolumeMultiplier(pos);

	if (volumeMultiplier > 0.0f)
	{
		s.setVolume(s.getVolume() * volumeMultiplier);
		addGameSound(s.play());
	}
}

void Bloodworks::playOneShotSound(sol::table& args)
{
	oneShotSoundManager->playSample(args);
}

float Bloodworks::getVolumeMultiplier(const Vec2& pos) const
{
	float dist = player->getPosition().distanceSquared(pos);
	if (dist < 1000.0f * 1000.0f)
	{
		float t = sqrtf(dist);
		t = (t - 100) / 900.0f;
		if (t < 0.0f)
		{
			t = 0.0f;
		}
		t = 1.0f - t;
		t = t*t;
		t = 0.25f + t * 0.75f;
		return t;
	}
	return 0.0f;
}

void Bloodworks::addGameSound(cSoundHandle& handle)
{
	gameSounds.push_back(handle);
	if (soundSpeed < 0.95f)
	{
		handle.setSpeed(soundSpeed);
	}
}

void Bloodworks::setSoundSpeed(float newSoundSpeed)
{
	soundSpeed = newSoundSpeed;
	if (soundSpeed >= 0.05f)
	{
		if (soundPaused)
		{
			soundPaused = false;
			for (auto& handle : gameSounds)
			{
				handle.resume();
				handle.setSpeed(soundSpeed);
			}
		}
		else
		{
			for (auto& handle : gameSounds)
			{
				handle.setSpeed(soundSpeed);
			}
		}
	}
	else
	{
		soundPaused = true;
		for (auto& handle : gameSounds)
		{
			handle.pause();
		}
	}
}

float Bloodworks::getSoundSpeed() const
{
	return soundSpeed;
}

bool Bloodworks::isMissionLoaded() const
{
	return missionController->isLoaded();
}

void Bloodworks::onGunReloaded(Gun* gun)
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			bonus->onReload(gun);
		}
	}
	for (auto& perk : usedPerks)
	{
		perk->onReload(gun);
	}
}

void Bloodworks::onMonsterDied(Monster* monster, float dropChance)
{
	for (auto& bonus : usedPerks)
	{
		bonus->onMonsterDied(monster);
	}
	for (auto& perk : usedPerks)
	{
		perk->onMonsterDied(monster);
	}

	monsterController->onMonsterDied(monster);
	missionController->onMonsterDied(monster);
	dropController->onMonsterDied(monster, dropChance);
}

void Bloodworks::showOptions()
{
	optionsPopup->show();
}

bool Bloodworks::isOptionsVisible() const
{
	return optionsPopup->isVisible();
}

void Bloodworks::onLevelUp()
{
	levelUpPopup->addLevel();
	if (config->getAutoOpenLevelupPopup())
	{
		levelUpPopup->show();
	}
	else
	{
		levelUpPopup->showLevelUpText();
	}
}

void Bloodworks::addSlaveWork(cSlaveWork* work)
{
	coral.getSlaveController()->addWork(work);
}

void Bloodworks::cancelSlaveWork(cSlaveWork* work)
{
	coral.getSlaveController()->cancelWork(work);
}

void Bloodworks::showMods()
{
	modWindow->setVisible(true, true);
}

void Bloodworks::loadMod(const std::string& path, bool loadOnlyModData)
{
	DirentHelper::Folder folder(path);
	cVector<DirentHelper::File> files = folder.getAllFiles(true);
	for (auto& f : files)
	{
		if (f.isTypeOf("json"))
		{
			nlohmann::json j;
			appendJson(j, f.folder + f.file);
			parseJson(j, f, loadOnlyModData);
		}
	}
}

void Bloodworks::updateVolume()
{
	coral.getSoundManager()->setGlobalVolume(config->getVolume());
}


void Bloodworks::updateMusicVolume()
{
	mainMenu->updateMusicVolume();
	missionController->updateMusicVolume();
}

void Bloodworks::addToActiveBonuses(Bonus* bonusToAdd)
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus == bonusToAdd)
		{
			return;
		}
	}
	activeBonuses.push_back(bonusToAdd);
}

void Bloodworks::removeFromActiveBonuses(Bonus *bonus)
{
}

void Bloodworks::onPlayerPickedGun(Gun *gun)
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			bonus->onPlayerPickedGun(gun);
		}
	}
	for (auto& perk : usedPerks)
	{
		perk->onPlayerPickedGun(gun);
	}
}

void Bloodworks::onPlayerPickedBonus(Bonus *bonus, const Vec2& pos)
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			bonus->onPlayerPickedBonus(bonus, pos);
		}
	}
	for (auto& perk : usedPerks)
	{
		perk->onPlayerPickedBonus(bonus, pos);
	}
}

int Bloodworks::onMonsterDamaged(Monster* monster, int damage, const Vec2& dir, sol::table& args)
{
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			damage = bonus->onMonsterDamaged(monster, damage, dir, args);
			if (damage <= 0)
			{
				return damage;
			}
		}
	}
	for (auto& perk : usedPerks)
	{
		damage = perk->onMonsterDamaged(monster, damage, dir, args);
		if (damage <= 0)
		{
			return damage;
		}
	}

	damage = monsterController->onMonsterDamaged(monster, damage, dir, args);
	return damage;
}

CollisionController* Bloodworks::getCollisionController() const
{
	return collisionController;
}

GameObjectTemplate* Bloodworks::getGameObjectTemplate(const std::string& templateName)
{
	return gameObjectTemplates[templateName];
}

void Bloodworks::clear()
{
	SAFE_DELETE(config);
	if (crashReporterWindow)
	{
		SAFE_DELETE(crashReporterWindow);
		return;
	}
	for (auto& animation : animationTemplates)
	{
		SAFE_DELETE(animation.second);
	}
	animationTemplates.clear();
	for (auto& particle : particles)
	{
		SAFE_DELETE(particle.second);
	}
	particles.clear();
	for (auto& laser : laserTemplates)
	{
		SAFE_DELETE(laser.second);
	}
	laserTemplates.clear();

	player->setGun(nullptr);
	SAFE_DELETE(bloodRenderable);
	SAFE_DELETE(bg);
	SAFE_DELETE(player);
	SAFE_DELETE(pausePostProcess);
	SAFE_DELETE(luaWorld);
	for (auto& gun : guns)
	{
		SAFE_DELETE(gun);
	}
	guns.clear();
	for (auto& bonus : bonuses)
	{
		SAFE_DELETE(bonus);
	}
	bonuses.clear();
	SAFE_DELETE(dropController);
	SAFE_DELETE(explosionController);

	for (auto& fg : fgs)
	{
		SAFE_DELETE(fg);
	}
	fgs.clear();

	for (auto& perk : perks)
	{
		SAFE_DELETE(perk);
	}
	perks.clear();

	SAFE_DELETE(optionsPopup);
	SAFE_DELETE(levelUpPopup);
	SAFE_DELETE(mainMenu);
	SAFE_DELETE(dropController);

	SAFE_DELETE(monsterController);
	SAFE_DELETE(bulletController);
	SAFE_DELETE(missionController);
	SAFE_DELETE(collisionController);
	SAFE_DELETE(oneShotSoundManager);
	SAFE_DELETE(modWindow);
	SAFE_DELETE(creditsWindow);

#ifdef HAS_BLOODWORKS_CHEATS
	SAFE_DELETE(bloodworksCheats);
#endif
}

void Bloodworks::reload()
{
	clear();
	initImplementation();
}

void Bloodworks::showCredits()
{
	creditsWindow->setVisible(true);
}

void Bloodworks::setMainMenuVisible()
{
	mainMenu->setVisible(true);
}

void Bloodworks::showCustomGames()
{
	loadMission("BossFight");
}

void Bloodworks::parseJson(nlohmann::json& j, DirentHelper::File& f, bool loadOnlyModData)
{
	if (j.count("type") == 0)
	{
		return;
	}
	std::string type = j["type"].get<std::string>();
	if (type == "mod")
	{
		modWindow->addInstalledMod(j, f);
	}

	if (loadOnlyModData || (j.count("disabled") && j["disabled"].get<bool>() == true) || modWindow->isPathDisabled(f.folder))
	{
		return;
	}

	static bool modsFolderDisabled = config->getBool("all_mods_disabled", false, "Set if you don't want to load mods");
	if (modsFolderDisabled)
	{
		if (beginsWith(f.folder, std::string("resources/mods/")))
		{
			return;
		}
	}

	if (type == "gun")
	{
		Gun *gun = new Gun(this, j, f);
		guns.push_back(gun);
	}
	else if (type == "bonus")
	{
		Bonus *bonus = new Bonus(this, j, f);
		bonuses.push_back(bonus);
	}
	else if (type == "particle")
	{
		cParticleTemplate *particleTemplate = new cParticleTemplate(j, f);
		particles[particleTemplate->getName()] = particleTemplate;
	}
	else if (type == "perk")
	{
		Perk *perk = new Perk(this, j, f);
		perks.push_back(perk);
	}
	else if (type == "monster")
	{
		monsterController->addMonsterTemplate(j, f);
	}
	else if (type == "mission")
	{
		missionController->addMission(j, f);
	}
	else if (type == "animation_template")
	{
		cAnimationTemplate *animation = new cAnimationTemplate(j, f);
		animationTemplates[animation->getName()] = animation;
	}
	else if (type == "game_object_template")
	{
		GameObjectTemplate *gameObjectTemplate = new GameObjectTemplate(j, f);
		gameObjectTemplates[gameObjectTemplate->getName()] = gameObjectTemplate;
	}
	else if (type == "json_list")
	{
		auto& elements = j["elements"];
		for (int i = 0; i < elements.size(); i++)
		{
			parseJson(elements.at(i), f);
		}
	}
}

BloodRenderable* Bloodworks::getBloodRenderable()
{
	return bloodRenderable;
}

bool Bloodworks::isCoorOutside(const Vec2& pos, float radius) const
{
	return pos.x - radius < mapRect.getMin().x || pos.y - radius <  mapRect.getMin().x || pos.x + radius > mapRect.getMax().x || pos.y + radius > mapRect.getMax().y;
}

bool Bloodworks::isCoorOutside(const Vec2& pos) const
{
	return mapRect.isOutside(pos);
}

void Bloodworks::addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage, float startTime)
{
	explosionController->addExplosion(pos, maxScale, scaleSpeed, minDamage, maxDamage, startTime);
}

void Bloodworks::addDrop(const Vec2& position)
{
	dropController->spawnDrop(position);
}

void Bloodworks::tick()
{
	ADD_SCOPED_TIME_PROFILER("Bloodworks::tick");

	if (crashReporterWindow)
	{
		crashReporterWindow->tick();
		if (crashReporterWindow->isClosed())
		{
			bool remember = crashReporterWindow->dontAskAgain();
			bool shouldSend = crashReporterWindow->sendClicked();
			if (shouldSend)
			{
				SendReport("in-game report", true);
			}
			cPackHelper::deleteFile(STD_OUTPUT_COPY);
			SAFE_DELETE(crashReporterWindow);
			initImplementation();
			if (remember)
			{
				config->setCrashAutoSendState(shouldSend ? 1 : 2);
			}
		}
		config->check();
		return;
	}

#ifdef HAS_BLOODWORKS_CHEATS
	bloodworksCheats->onTick();
#endif

	lua["dt"] = timer.getDt();
	lua["time"] = timer.getTime();
	lua["timeScale"] = getSlowdown();

	if (levelUpPopup->isVisible() == false && levelUpPopup->getWaitingLevels() > 0 && player->isActive() && mapper.isKeyDown(GameKey::LevelUp))
	{
		levelUpPopup->show();
	}

	luaWorld->tick();

	if (isMissionLoaded() == false)
	{
		mainMenu->tick(optionsPopup->isVisible() || modWindow->isVisible() || creditsWindow->isVisible());
		optionsPopup->tick();
		modWindow->tick();
		creditsWindow->tick();
	}

	oneShotSoundManager->tick();

	for (int i = 0; i < gameSounds.size(); i++)
	{
		if (gameSounds[i].isFinished())
		{
			gameSounds.swapToTailRemove(i);
			i--;
		}
	}

	bloodRenderable->tick();
	player->tick();
	tickCamera();

	for (int i = 0; i < orphanParticles.size(); i++)
	{
		if (orphanParticles[i]->hasParticle() == false)
		{
			SAFE_DELETE(orphanParticles[i]);
			orphanParticles.swapToTailRemove(i);
			i--;
		}
	}
	for (int i = 0; i < activeBonuses.size(); i++)
	{
		if (activeBonuses[i]->isActive() == false)
		{
			activeBonuses.swapToTailRemove(i);
			i--;
		}
	}
	for (auto& bonus : activeBonuses)
	{
		if (bonus->isActive())
		{
			bonus->onTick();
		}
	}
	for (auto& perk : usedPerks)
	{
		perk->onTick();
	}
	monsterController->tick();
	bulletController->tick();
	missionController->tick();
	dropController->tick();
	explosionController->tick();
	levelUpPopup->tick();
	tickGameSlowdown();

	if (mapper.isKeyPressed(GameKey::Fullscreen))
	{
		coral.setFullScreen(!coral.isFullScreen());
		config->setFullScreen(coral.isFullScreen());
		if (coral.isFullScreen())
		{
			cameraZoom = 0.8f;
		}
		else
		{
			cameraZoom = 1.0f;
		}
	}

	config->check();

	if (mapper.isKeyPressed(GameKey::Back))
	{
		if (missionController->canExit())
		{
			gotoMainMenu();
		}
	}

}

void Bloodworks::tickCamera()
{
	if (player->isVisible())
	{
		AARect playerRect(player->getPosition(), player->getPosition());
		playerRect.addThreshold(50.0f);
		playerRect.clampPos(cameraCenterPos);

		Vec2 playerAimDir = player->getCrosshairPos();
		float playerAimLength = playerAimDir.normalize();
		if (playerAimLength > 30.0f)
		{
			float l = playerAimLength - 30.0f;

			cameraPos = cameraCenterPos + playerAimDir * l * 0.1f;
		}
		else
		{
			cameraPos = cameraCenterPos;
		}
	}

	float approachToEdgeSize = 100.0f;
	float horizontalMaxMove = getScreenDimensions().x * 0.5f - approachToEdgeSize;
	float verticalMaxMove = getScreenDimensions().y * 0.5f - approachToEdgeSize;

	horizontalMaxMove *= cameraZoom;
	verticalMaxMove *= cameraZoom;

	AARect cameraRect = mapRect;
	cameraRect.addThreshold(-horizontalMaxMove, -verticalMaxMove);
	if (cameraRect.isValidSafeSafe())
	{
		cameraRect.clampPos(cameraPos);
	}
}

void Bloodworks::tickGameSlowdown()
{
	if (levelUpPopup->isVisible() == false && missionController->isLoaded() && mapper.isKeyPressed(GameKey::Pause))
	{
		paused = !paused;
		if (paused)
		{
			pausePostProcess->setEnabled(true);
		}
	}

	if (paused && pauseSlowdown > 0.0f)
	{
		pauseSlowdown -= timer.getNonSlowedDt() * 2.0f;
		if (pauseSlowdown < 0.0f)
		{
			pauseSlowdown = 0.0f;
		}
		pausePostProcess->setShaderWeight(1.0f - pauseSlowdown);
	}
	else if (!paused && pauseSlowdown < 1.0f)
	{
		pauseSlowdown += timer.getNonSlowedDt() * 2.0f;
		if (pauseSlowdown >= 1.0f)
		{
			pauseSlowdown = 1.0f;
			pausePostProcess->setEnabled(false);
		}
		pausePostProcess->setShaderWeight(1.0f - pauseSlowdown);
	}

	float gamePlaySlowdown = missionController->getGameSpeedMultiplier();

	float totalSlowdown = pauseSlowdown * gamePlaySlowdown;
	if (fabs(totalSlowdown - slowdown) > 0.01f)
	{
		float newSoundSpeed = pauseSlowdown * gamePlaySlowdown;
		newSoundSpeed = round(newSoundSpeed * 10.0f) / 10.0f;
		if (abs(newSoundSpeed - soundSpeed) > 0.05f)
		{
			setSoundSpeed(newSoundSpeed);
		}

		setSlowdown(pauseSlowdown * gamePlaySlowdown);
	}
}

void Bloodworks::render()
{
	if (crashReporterWindow)
	{
		return;
	}
#ifdef HAS_BLOODWORKS_CHEATS
	bloodworksCheats->onRender();
#endif

}
