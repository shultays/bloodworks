#include "Gun.h"
#include "Bloodworks.h"
#include "cGlobals.h"
#include "BulletController.h"
#include "Player.h"
#include "Bullet.h"
#include "cRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "LaserRenderable.h"
#include "cSound.h"
#include "Monster.h"
#include "BloodworksControls.h"

Gun::Gun(Bloodworks *bloodworks, nlohmann::json& j)
{
	this->bloodworks = bloodworks;

	lastShootSoundTime = timer.getTime();

	name = j["name"].get<std::string>();
	artFolder = j["artFolder"].get<std::string>();
	fixFolderPath(artFolder);

	if (j.count("hideSpread"))
	{
		hideSpread = j["hideSpread"].get<bool>();
	}
	else
	{
		hideSpread = false;
	}

	if (j.count("spawnChance"))
	{
		spawnChance = j["spawnChance"].get<float>();
	}
	else
	{
		spawnChance = 1.0f;
	}

	if (j.count("shootParticleColor"))
	{
		Vec4 color(1.0);
		if (j["shootParticleColor"].size() == 4)
		{
			color.a = j["shootParticleColor"].at(3).get<float>();
		}
		color.r = j["shootParticleColor"].at(0).get<float>();
		color.g = j["shootParticleColor"].at(1).get<float>();
		color.b = j["shootParticleColor"].at(2).get<float>();
		shootParticleColor = color;
	}
	else
	{
		shootParticleColor = Vec4(1.0f, 0.4f, 0.2f, 1.0f);
	}
	if (j.count("bulletTexture"))
	{
		bulletTexturePath = artFolder + j["bulletTexture"].get<std::string>();
		bulletTexture = resources.getTexture(bulletTexturePath);
	}
	if (j.count("bulletMeshShift"))
	{
		bulletMeshShift.w = j["bulletMeshShift"].at(0).get<float>();
		bulletMeshShift.h = j["bulletMeshShift"].at(1).get<float>();
	}
	else
	{
		bulletMeshShift.setZero();
	}
	iconPath = artFolder + j["icon"].get<std::string>();

	bulletSize.w = j["bulletSize"].at(0).get<float>();
	bulletSize.h = j["bulletSize"].at(1).get<float>();
	bulletRadius = j["bulletRadius"].get<float>();
	bulletSpeed = j["bulletSpeed"].get<float>();

	damage = IntVec2(j["bulletDamage"].at(0).get<int>(), j["bulletDamage"].at(1).get<int>());

	std::string scriptFilePath = j["scriptFile"].get<std::string>();
	fixFilePath(scriptFilePath);
	scriptName = j["scriptName"].get<std::string>();
	scriptTable = lua[scriptName] = lua.create_table();
	data = lua.create_table();
	id = bloodworks->getUniqueId();

	spreadAngle = 0.0f;
	crosshairDistance = 400.0f;

	lua.script_file(scriptFilePath);

	if (j.count("isLaser") && j["isLaser"].get<bool>() == true)
	{
		LaserTemplate *laserTemplate = new LaserTemplate(game, j);
		bloodworks->addLaserTemplate(laserTemplate);
		laser = new LaserRenderable(bloodworks, laserTemplate);
		laser->setWorldMatrix(Mat3::identity());
		bloodworks->addRenderable(laser, PLAYER + 1);
		laser->setVisible(false);
	}
	else
	{
		laser = nullptr;
	}

	if (j.count("firingSound"))
	{
		gunShootSound.loadSample(j["firingSound"]);
	}

	if (j.count("bulletHitSound"))
	{
		bulletHitSound.loadSample(j["bulletHitSound"]);
	}

	if (j.count("reloadBeginSound"))
	{
		reloadBeginSound.loadSample(j["reloadBeginSound"]);
	}
	else
	{
		reloadBeginSound.setSample(resources.getSoundSample("resources/sounds/reload_begin.ogg"));
	}

	if (j.count("reloadEndSound"))
	{
		reloadEndSound.loadSample(j["reloadEndSound"]);
	}
	else
	{
		reloadEndSound.setSample(resources.getSoundSample("resources/sounds/reload_end.ogg"));
	}

	if (j.count("maxAmmo"))
	{
		maxAmmo = j["maxAmmo"].get<int>();
	}
	else
	{
		maxAmmo = 3;
	}

	if (j.count("reloadTime"))
	{
		reloadTime = j["reloadTime"].get<float>();
	}
	else
	{
		reloadTime = 2.0f;
	}

	buffedMaxAmmo = currentAmmo = getMaxAmmo();
	reloading = false;

	scriptTable["init"](this);
}

void Gun::stop()
{
	setTriggered(false);
	if (laser)
	{
		laser->setVisible(false);

		if (gunShootSoundHandle.isValid())
		{
			gunShootSoundHandle.stop();
		}
	}
}

void Gun::start()
{
	isTriggered = false;
	currentAmmo = getMaxAmmo();
	buffedMaxAmmo = currentAmmo;
	reloading = false;
}

void Gun::tick(float dt)
{
	int newMaxAmmo = getMaxAmmo();
	if (buffedMaxAmmo < newMaxAmmo)
	{
		currentAmmo += (newMaxAmmo - buffedMaxAmmo);
	}
	if (currentAmmo > newMaxAmmo)
	{
		currentAmmo = newMaxAmmo;
	}
	buffedMaxAmmo = newMaxAmmo;
	if (currentAmmo < buffedMaxAmmo && mapper.isKeyPressed(GameKey::Reload))
	{
		reload();
	}
	reloadSpeedMultiplier.tick();
	if (reloading)
	{
		float playerReloadSpeedMult = bloodworks->getPlayer()->getReloadSpeedMultiplier();
		remainingReload -= timer.getDt() * reloadSpeedMultiplier.getBuffedValue() * playerReloadSpeedMult;

		if (reloadEnding == false && remainingReload < 1.0f)
		{
			reloadEnding = true;

			if (reloadEndSound.isValid())
			{
				bloodworks->addGameSound(reloadEndSound.play());
			}
		}

		if (remainingReload < 0.0)
		{
			reloading = false;
			currentAmmo = buffedMaxAmmo;

			if (scriptTable["onReloadEnded"])
			{
				scriptTable["onReloadEnded"](this);
			}
		}
	}

	scriptTable["onTick"](this);

	if (laser && timer.getDt() > 0.0f)
	{
		if (gunShootSound.isValid())
		{
			if (laser->isVisible() && gunShootSoundHandle.isValid() == false)
			{
				gunShootSoundHandle = gunShootSound.play();
				gunShootSoundHandle.setLooped(true);
				bloodworks->addGameSound(gunShootSoundHandle);
			}
			else if (!laser->isVisible() && gunShootSoundHandle.isValid())
			{
				gunShootSoundHandle.stop();
				gunShootSoundHandle.clear();
			}
		}
	}
}

float Gun::getMaxCrosshairDistance()
{
	return crosshairDistance;
}

float Gun::getSpreadAngle()
{
	return spreadAngle;
}

int Gun::getId()
{
	return id;
}

sol::table& Gun::getScriptTable()
{
	return scriptTable;
}

const std::string& Gun::getIconPath() const
{
	return iconPath;
}

bool Gun::spreadVisible() const
{
	return !hideSpread;
}

void Gun::setTriggered(bool triggered)
{
	this->isTriggered = triggered;
}

const Vec4& Gun::getShootingParticleColor() const
{
	return shootParticleColor;
}

void Gun::updateLaser(const Vec2& pos, float angle)
{
	laser->setPositionAndAngle(pos, angle);
}

void Gun::reset()
{
	data = lua.create_table();
	id = bloodworks->getUniqueId();

	spreadAngle = 0.0f;

	currentAmmo = maxAmmo;
	reloading = false;

	scriptTable["init"](this);

	if (laser)
	{
		laser->setVisible(false);
	}
}

void Gun::onBulletHit(Bullet *bullet, Monster* monster)
{
	if (scriptTable["onBulletHit"])
	{
		scriptTable["onBulletHit"](this, bullet, monster);
	}

	if (bulletHitSound.isValid())
	{
		bloodworks->playSoundAtMap(bullet->getPosition(), bulletHitSound);
	}
}

void Gun::addAmmo()
{
	currentAmmo++;
	if (currentAmmo > maxAmmo)
	{
		currentAmmo = maxAmmo;
	}
}

void Gun::consumeAmmo()
{
	if (currentAmmo > 0)
	{
		currentAmmo--;
		if (currentAmmo <= 0)
		{
			currentAmmo = 0;
			reload();
		}
	}
}

void Gun::reload()
{
	if (reloading == false)
	{
		bloodworks->onGunReloaded(this);
		currentAmmo = 0;
		reloading = true;
		reloadEnding = false;
		remainingReload = reloadTime;
		if (reloadBeginSound.isValid())
		{
			bloodworks->addGameSound(reloadBeginSound.play());
		}

		if (scriptTable["onReloadStarted"])
		{
			scriptTable["onReloadStarted"](this);
		}
	}
}

int Gun::getMaxAmmo()
{
	return bloodworks->getPlayer()->getBuffedClipSize(maxAmmo);
}

float Gun::getSpawnChance()
{
	if (dynamicSpawnChance)
	{
		return dynamicSpawnChance();
	}
	return spawnChance;
}

int Gun::getCurrentAmmo() const
{
	return currentAmmo;
}

bool Gun::isReloading() const
{
	return reloading;
}

float Gun::getReloadPercentage() const
{
	return remainingReload / reloadTime;
}

Gun::~Gun()
{
	bulletTexture = nullptr;
	SAFE_DELETE(laser);
}

Bullet* Gun::addBullet()
{
	Vec2 dir = bloodworks->getPlayer()->getAimDir();
	Bullet *bullet = new Bullet(bloodworks, this);
	Player *player = bloodworks->getPlayer();
	bullet->pos = player->getGunPos();
	bullet->moveSpeed = bulletSpeed;
	bullet->moveAngle = player->getAimDir().toAngle() + randFloat(-spreadAngle, spreadAngle);
	bullet->radius = bulletRadius;
	bullet->damage = randInt(damage.x, damage.y);
	if (bulletTexture != nullptr)
	{
		cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, bulletTexturePath, "resources/default");
		renderable->setWorldMatrix(Mat3::scaleMatrix(bulletSize).translateBy(bulletMeshShift));
		bullet->addRenderable(renderable);
	}
	bloodworks->getBulletController()->addBullet(bullet);

	bloodworks->onAddedGunBullet(this, bullet);

	if (gunShootSound.isValid() && lastShootSoundTime + 0.1f < timer.getTime())
	{
		lastShootSoundTime = timer.getTime();
		bloodworks->addGameSound(gunShootSound.play());
	}

	return bullet;
}
