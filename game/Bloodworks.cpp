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

#include <sstream>


void appendJson(nlohmann::json& j, const std::string& fileName)
{
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

void Bloodworks::init()
{
	luaWorld = new BloodworksLuaWorld(this);

	lua.script_file("resources/helpers.lua");
	lua.script_file("resources/guns/helpers.lua");
	lua.script_file("resources/monsters/helpers.lua");

	lua["time"] = timer.getTime();

	dropController = new DropController(this);
	monsterController = new MonsterController(this);
	bulletController = new BulletController(this);
	missionController = new MissionController(this);

	DirentHelper::Folder folder("./resources");
	std::vector<DirentHelper::File> files = folder.getAllFiles(true);
	for (auto& f : files)
	{
		if (f.isTypeOf("json"))
		{
			nlohmann::json j;
			appendJson(j, f.folder + f.file);
			std::string type = j["type"].get<std::string>();
			if (type == "gun")
			{
				Gun *gun = new Gun(this, j);
				guns.push_back(gun);
			}
			else if (type == "bonus")
			{
				Bonus *bonus = new Bonus(j);
				bonuses.push_back(bonus);
			}
			else if (type == "particle")
			{
				cParticleTemplate *particleTemplate = new cParticleTemplate(j);
				particles[particleTemplate->getName()] = particleTemplate;
			}
			else if (type == "perk")
			{
				Perk *perk = new Perk(j);
				perks.push_back(perk);
			}
			else if (type == "monster")
			{
				monsterController->addMonsterTemplate(j);
			}
			else if (type == "mission")
			{
				missionController->addMission(j);
			}
		}
	}


	cShaderShr shader = resources.getShader("resources/defaultWithUVScale.vs", "resources/default.ps");
	int uvBegin = shader->addUniform("uvBegin", TypeVec2).index;
	int uvSize = shader->addUniform("uvSize", TypeVec2).index;

	bg = new cTexturedQuadRenderable(this, "resources/bg.png", "resources/default");
	bg->setWorldMatrix(Mat3::scaleMatrix(2048.0f));
	bg->setShader(shader);
	bg->setUniform(uvBegin, Vec2(0.0f));
	bg->setUniform(uvSize, Vec2(4.0f));
	addRenderable(bg, BACKGROUND);

	cTexturedQuadRenderable *fg;
	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, mapSize.y * 0.5f - 40.0f).translateBy(mapBegin.x, 0.0f));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.0f, 0.5f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, mapSize.y * 0.5f - 40.0f).translateBy(mapEnd.x, 0.0f));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.8f, 0.5f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(mapSize.x * 0.5f - 40.0f, 40.0f).translateBy(0.0f, mapBegin.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.5f, 0.8f));
	fg->setUniform(uvSize, Vec2(0.0f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(mapSize.x * 0.5f - 40.0f, 40.0f).translateBy(0.0f, mapEnd.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.5f, 0.0f));
	fg->setUniform(uvSize, Vec2(0.0f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapBegin.x, mapBegin.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.0f, 0.8f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapBegin.x, mapEnd.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.0f, 0.0f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapEnd.x, mapBegin.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.8f, 0.8f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, "resources/fg_black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(40.0f, 40.0f).translateBy(mapEnd.x, mapEnd.y));
	fg->setShader(shader);
	fg->setUniform(uvBegin, Vec2(0.8f, 0.0f));
	fg->setUniform(uvSize, Vec2(0.2f, 0.2f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	fg = new cTexturedQuadRenderable(this, "resources/black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f, 600.0f + mapSize.y).translateBy(mapBegin.x - 640.0f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	fg = new cTexturedQuadRenderable(this, "resources/black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f, 600.0f + mapSize.y).translateBy(mapEnd.x + 640.0f, 0.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);


	fg = new cTexturedQuadRenderable(this, "resources/black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f + mapSize.x, 600.0f).translateBy(0.0f, mapBegin.y - 640.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	fg = new cTexturedQuadRenderable(this, "resources/black.png", "resources/default");
	fg->setWorldMatrix(Mat3::scaleMatrix(600.0f + mapSize.x, 600.0f).translateBy(0.0f, mapEnd.y + 640.0f));
	addRenderable(fg, FOREGROUND);
	fgs.push_back(fg);

	input.hideMouse();

	player = new Player(this);
	player->setGun(guns[0]);
	
	bloodRenderable = new BloodRenderable(this);
	bloodRenderable->init();
	addRenderable(bloodRenderable, BACKGROUND + 1);

	explosionController = new ExplosionController(this);

	pausePostProcess = new cPostProcess();
	pausePostProcess->init(this, resources.getShader("resources/post_process/default.vs", "resources/post_process/pause.ps"), 200);
	pausePostProcess->setShaderWeight(0.0f);
	pausePostProcess->setEnabled(false);

	levelUpPopup = new LevelUpPopup(this);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	mainMenu = new MainMenu(this);

	if (coral.isDebuggerPresent())
	{
		loadMission("Survival");
	}
	else
	{
		mainMenu->setVisible(true);
		showFps = false;
	}
}

Bloodworks::Bloodworks()
{
	nextUniqueId = 1;

	mapSize = 2000.0f;
	mapBegin = -mapSize*0.5f;
	mapEnd = mapBegin + mapSize;

	postProcessEndLevel = (GUI + FOREGROUND) / 2;

	targetGamePlaySlowdown = pauseSlowdown = gamePlaySlowdown = 1.0f;
	paused = false;

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;

	showFps = true;

	cameraCenterPos.setZero();
}

Bloodworks::~Bloodworks()
{
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
	// SAFE_DELETE(luaWorld); TODO fix
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

	SAFE_DELETE(levelUpPopup);
	SAFE_DELETE(dropController);

	SAFE_DELETE(monsterController);
	SAFE_DELETE(bulletController);
	SAFE_DELETE(missionController);

	SAFE_DELETE(mainMenu);
}

void Bloodworks::onAddedGunBullet(Gun *gun, Bullet *bullet)
{
	for (auto& perk : usedPerks)
	{
		perk->onAddGunBullet(gun, bullet);
	}
}

void Bloodworks::multiplyGameSpeed(float multiplier)
{
	targetGamePlaySlowdown *= multiplier;
	if (targetGamePlaySlowdown > 0.99f)
	{
		targetGamePlaySlowdown = 1.0f;
	}
}

void Bloodworks::openLevelupPopup()
{
	levelUpPopup->show();
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

std::vector<Perk*> Bloodworks::getAvailablePerks() const
{
	std::vector<Perk*> availablePerks;
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

int Bloodworks::onPlayerDamaged(int damage, sol::table& params)
{
	for (auto& perk : usedPerks)
	{
		if (damage <= 0)
		{
			return damage;
		}
		damage = perk->onPlayerDamaged(damage, params);
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
}

void Bloodworks::clearMission()
{
	gamePlaySlowdown = targetGamePlaySlowdown = 1.0f;
	setSlowdown(1.0f);

	player->reset();
	monsterController->reset();
	bloodRenderable->reset();
	dropController->reset();
	bulletController->reset();
	explosionController->reset();
	missionController->reset();

	std::vector<cPostProcess*> toRemove;

	for (int i=0; i<postProcesses.size(); i++)
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

	for (auto& perk : usedPerks)
	{
		perk->reset();
	}
	usedPerks.clear();

	for (auto& bonus : bonuses)
	{
		bonus->reset();
	}
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
	mainMenu->setVisible(true);
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

	player->setVisible(true);

	for (auto& gun : guns)
	{
		if (gun->getName() == "Railgun")
		{
			player->setGun(gun);
		}
		if (gun->getName() == "Laser Machine Gun")
		{
			player->setSecondaryGun(gun);
		}
	}
	missionController->loadMission(mission);
	return true;
}

void Bloodworks::onPlayerDied()
{
	for (auto& perk : usedPerks)
	{
		perk->onPlayerDied();
	}
	missionController->onPlayerDied();
}

void Bloodworks::playSoundAtMap(const Vec2& pos, cSoundSampleShr s, float volume)
{
	float dist = player->getPosition().distanceSquared(pos);
	if (dist < 700.0f * 700.0f)
	{
		float t = sqrtf(dist);
		t = t / 700.0f;
		if (t < 0.0f)
		{
			t = 0.0f;
		}
		t = 1.0f - t;
		t = t*t;
		t = 0.1f + t * 0.9f;
		s->play(t * volume);
	}
}

void Bloodworks::tickCamera()
{
	if (cameraCenterPos.x > player->getPosition().x + 50.0f)
	{
		cameraCenterPos.x = player->getPosition().x + 50;
	}
	else if (cameraCenterPos.x < player->getPosition().x - 50.0f)
	{
		cameraCenterPos.x = player->getPosition().x - 50;
	}
	if (cameraCenterPos.y > player->getPosition().y + 50.0f)
	{
		cameraCenterPos.y = player->getPosition().y + 50;
	}
	else if (cameraCenterPos.y < player->getPosition().y - 50.0f)
	{
		cameraCenterPos.y = player->getPosition().y - 50;
	}

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

	float approachToEdgeSize = 100.0f;
	float horizontalMaxMove = getScreenDimensions().x * 0.5f - approachToEdgeSize;
	float verticalMaxMove = getScreenDimensions().y * 0.5f - approachToEdgeSize;

	horizontalMaxMove *= cameraZoom;
	verticalMaxMove *= cameraZoom;

	if (cameraPos.x < mapBegin.x + horizontalMaxMove)
	{
		cameraPos.x = mapBegin.x + horizontalMaxMove;
	}
	else if (cameraPos.x > mapEnd.x - horizontalMaxMove)
	{
		cameraPos.x = mapEnd.x - horizontalMaxMove;
	}

	if (cameraPos.y < mapBegin.y + verticalMaxMove)
	{
		cameraPos.y = mapBegin.y + verticalMaxMove;
	}
	else if (cameraPos.y > mapEnd.y - verticalMaxMove)
	{
		cameraPos.y = mapEnd.y - verticalMaxMove;
	}

}

void Bloodworks::tickGameSlowdown()
{
	if (levelUpPopup->isVisible() == false && input.isKeyPressed(key_p))
	{
		paused = !paused;
		if (paused)
		{
			pausePostProcess->setEnabled(true);
		}
	}

	bool changeSlowdown = false;
	if (paused && pauseSlowdown > 0.0f)
	{
		pauseSlowdown -= timer.realDt * 2.0f;
		if (pauseSlowdown < 0.0f)
		{
			pauseSlowdown = 0.0f;
		}
		pausePostProcess->setShaderWeight(1.0f - pauseSlowdown);
		changeSlowdown = true;
	}
	else if (!paused && pauseSlowdown < 1.0f)
	{
		pauseSlowdown += timer.realDt * 2.0f;
		if (pauseSlowdown >= 1.0f)
		{
			pauseSlowdown = 1.0f;
			pausePostProcess->setEnabled(false);
		}
		pausePostProcess->setShaderWeight(1.0f - pauseSlowdown);
		changeSlowdown = true;
	}

	if (gamePlaySlowdown > targetGamePlaySlowdown)
	{
		gamePlaySlowdown -= timer.realDt;
		if (gamePlaySlowdown < targetGamePlaySlowdown)
		{
			gamePlaySlowdown = targetGamePlaySlowdown;
		}
		changeSlowdown = true;
	}
	else if (gamePlaySlowdown < targetGamePlaySlowdown)
	{
		gamePlaySlowdown += timer.realDt;
		if (gamePlaySlowdown > targetGamePlaySlowdown)
		{
			gamePlaySlowdown = targetGamePlaySlowdown;
		}
		changeSlowdown = true;
	}

	if (changeSlowdown)
	{
		setSlowdown(pauseSlowdown * gamePlaySlowdown);
	}

	if (input.isKeyDown(key_m))
	{
		setSlowdown(0.1f);
	}
}

BloodRenderable* Bloodworks::getBloodRenderable()
{
	return bloodRenderable;
}

bool Bloodworks::isCoorOutside(const Vec2& pos, float radius) const
{
	return pos.x + radius < mapBegin.x || pos.y + radius < mapBegin.x || pos.x - radius> mapEnd.x || pos.y - radius> mapEnd.y;
}

bool Bloodworks::isCoorOutside(const Vec2& pos) const
{
	return pos.x < mapBegin.x || pos.y < mapBegin.x || pos.x > mapEnd.x || pos.y > mapEnd.y;
}


void Bloodworks::addExplosion(const Vec2& pos, float maxScale, float scaleSpeed, int minDamage, int maxDamage)
{
	explosionController->addExplosion(pos, maxScale, scaleSpeed, minDamage, maxDamage);
}

void Bloodworks::addDrop(const Vec2& position)	
{
	dropController->addDrop(position);
}

void Bloodworks::tick()
{
	mainMenu->tick();

	if (input.isKeyPressed(key_n))
	{
		showFps = !showFps;
		if (showFps == false)
		{
			debugRenderer.removeText(0);
			debugRenderer.removeText(1);
		}
	}

	if (showFps)
	{
		tickCount++;
		if (timer.getRealTime() - lastSetTickTime > 1.0f)
		{
			lastSetTickTime += 1.0f;
			std::stringstream ss;
			ss << "FPS " << tickCount << " Monster " << monsterController->getMonsterCount();
			debugRenderer.addText(0, ss.str(), 5.0f, -24.0f, FLT_MAX, Vec4(1.0f), 24.0f, TextAlignment::left, RenderableAlignment::topLeft);

			tickCount = 0;
		}
	}

	lua["dt"] = timer.getDt();
	lua["time"] = timer.getTime();
	lua["timeScale"] = getSlowdown();

	if (input.isKeyPressed(key_2) && perks[0]->isTakenFully() == false)
	{
		perks[8]->takeLevel();
		usedPerks.push_back(perks[8]);
	}

	if (input.isKeyPressed(key_3))
	{
		bonuses[0]->spawnAt(player->getPosition());
	}

	if (input.isKeyPressed(key_4))
	{
		for (int i = 0; i < guns.size(); i++)
		{
			dropController->createGun(player->getPosition() + Vec2(-100, i * 50.0f - guns.size() * 25.0f), i);
		}

		for (int i = 0; i < bonuses.size(); i++)
		{
			dropController->createBonus(player->getPosition() + Vec2(100, i * 50.0f - bonuses.size() * 25.0f), i);
		}
	}

	if (input.isKeyPressed(key_5) && levelUpPopup->isVisible() == false)
	{
		player->doLevelup();
	}
	bloodRenderable->tick();
	missionController->tick();

	player->tick();

	for (auto& perk : usedPerks)
	{
		perk->onTick();
	}

	tickCamera();
	monsterController->tick();
	bulletController->tick();
	dropController->tick();
	explosionController->tick();
	levelUpPopup->tick();
	tickGameSlowdown();

	if (input.isKeyPressed(key_f10))
	{
		coral.setFullScreen(!coral.isFullScreen());
		if (coral.isFullScreen())
		{
			cameraZoom = 0.8f;
		}
		else
		{
			cameraZoom = 1.0f;
		}
	}
}

void Bloodworks::render()
{
	if (showFps)
	{
		renderCount++;
		if (timer.getRealTime() - lastSetRenderTime > 1.0f)
		{
			lastSetRenderTime += 1.0f;
			std::stringstream ss;
			ss << "Render " << renderCount;
			debugRenderer.addText(1, ss.str(), 5.0f, -24.0f * 2.0f, FLT_MAX, Vec4(1.0f), 24.0f, TextAlignment::left, RenderableAlignment::topLeft);

			renderCount = 0;
		}
	}
}
