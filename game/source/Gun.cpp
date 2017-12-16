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
#include "StripLaserRenderable.h"

Gun::Gun(Bloodworks *bloodworks, nlohmann::json& j, const DirentHelper::File& file)
{
	this->bloodworks = bloodworks;

	path = file.folder;

	lastShootSoundTime = timer.getTime();

	name = j["name"].get<std::string>();

	if (j.count("hideSpread"))
	{
		hideSpread = j["hideSpread"].get<bool>();
	}
	else
	{
		hideSpread = false;
	}
	if (j.count("ultimate"))
	{
		ultimate = j["ultimate"].get<bool>();
	}
	else
	{
		ultimate = false;
	}

	if (j.count("maxSoundPlayInterval"))
	{
		maxSoundPlayInterval = j["maxSoundPlayInterval"].get<float>();
	}
	else
	{
		maxSoundPlayInterval = 0.1f;
	}

	if (j.count("spawnChance"))
	{
		spawnChance = j["spawnChance"].get<float>();
	}
	else
	{
		spawnChance = 1.0f;
	}
	showShootAnimation = true;
	if (j.count("showShootAnimation"))
	{
		showShootAnimation = j["showShootAnimation"].get<bool>();
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
		shootParticleColor = Vec4(1.0f, 0.4f, 0.2f, 0.6f);
	}
	if (j.count("bulletTexture"))
	{
		bulletTexturePath = file.folder + j["bulletTexture"].get<std::string>();
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
	iconPath = file.folder + j["icon"].get<std::string>();

	if (j.count("bulletSize"))
	{
		bulletSize.w = j["bulletSize"].at(0).get<float>();
		bulletSize.h = j["bulletSize"].at(1).get<float>();
	}

	bulletRadius = 4.0f;
	if (j.count("bulletRadius"))
	{
		bulletRadius = j["bulletRadius"].get<float>();
	}
	bulletSpeed = 150.0f;
	if (j.count("bulletSpeed"))
	{
		bulletSpeed = j["bulletSpeed"].get<float>();
	}
	damage = IntVec2(10, 20);
	if (j.count("bulletDamage"))
	{
		damage = IntVec2(j["bulletDamage"].at(0).get<int>(), j["bulletDamage"].at(1).get<int>());
	}

	std::string scriptFilePath = file.folder + j["scriptFile"].get<std::string>();
	fixFilePath(scriptFilePath);
	scriptName = j["scriptName"].get<std::string>();
	scriptTable = lua[scriptName] = lua.create_table();
	std::string folder = file.folder;
	fixFolderPath(folder);
	scriptTable["basePath"] = folder;
	data = lua.create_table();
	newBulletData = lua.create_table();
	id = bloodworks->getUniqueId();

	spreadAngle = 0.0f;
	crosshairDistance = 400.0f;
	if (j.count("crosshairDistance"))
	{
		crosshairDistance = j["crosshairDistance"].get<float>();
	}

	lua.script_file(scriptFilePath);

	gunShootSoundFadein = 0.1f;
	gunShootSoundFadeout = 0.1f;
	playGunShootSound = false;
	gunShootSoundCurVolume = 0.0f;

	bulletLevelModifier = 0;
	if (j.count("bulletLevelModifier"))
	{
		bulletLevelModifier = j["bulletLevelModifier"].get<int>();
	}

	if (j.count("isLaser") && j["isLaser"].get<bool>() == true)
	{
		LaserTemplate *laserTemplate = new LaserTemplate(game, j, file);
		bloodworks->addLaserTemplate(laserTemplate);
		laser = new LaserRenderable(bloodworks, laserTemplate);
		laser->setWorldMatrix(Mat3::identity());
		bloodworks->addRenderable(laser, PLAYER - 1 + bulletLevelModifier);
		laser->setVisible(false);
	}
	else
	{
		laser = nullptr;
	}

	if (j.count("stripLaserParticle"))
	{
		stripLaser = new StripLaserRenderable(bloodworks, j["stripLaserParticle"].get<std::string>(), PLAYER - 1 + bulletLevelModifier);
	}
	else
	{
		stripLaser = nullptr;
	}

	gunShootSoundContinuous = false;
	if (j.count("firingSoundContinuous"))
	{
		gunShootSoundContinuous = j["firingSoundContinuous"].get<bool>();
	}
	if (j.count("firingSound"))
	{
		gunShootSound.loadSample(j["firingSound"], file);
	}
	if (j.count("firingSoundFadein"))
	{
		gunShootSoundFadein = j["firingSoundFadein"].get<float>();
	}
	if (j.count("firingSoundFadeout"))
	{
		gunShootSoundFadeout = j["firingSoundFadeout"].get<float>();
	}

	if (j.count("bulletHitSound"))
	{
		bulletHitSound.loadSample(j["bulletHitSound"], file);
	}

	if (j.count("reloadBeginSound"))
	{
		reloadBeginSound.loadSample(j["reloadBeginSound"], file);
	}
	else
	{
		reloadBeginSound.setSample(resources.getSoundSample("resources/sounds/reload_begin.ogg"));
	}

	if (j.count("reloadEndSound"))
	{
		reloadEndSound.loadSample(j["reloadEndSound"], file);
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

	if (bloodworks->IsGUIHidden())
	{
		reloadTime = 0.0f;
	}

	if (j.count("bulletLifeTime"))
	{
		bulletLifeTime = j["bulletLifeTime"].get<float>();
	}
	else
	{
		bulletLifeTime = -1.0f;
	}


	dynamicSpawnChanceCall = scriptTable["spawnChance"];

	onTickCall = scriptTable["onTick"];
	onBulletTickCall = scriptTable["onBulletTick"];
	onBulletHitCall = scriptTable["onBulletHit"];
	onReloadStartCall = scriptTable["onReloadStart"];
	onReloadEndCall = scriptTable["onReloadEnd"];
	onPlayerDamagedCall = scriptTable["onPlayerDamaged"];

	sol::function onReloadStart;

	buffedMaxAmmo = currentAmmo = getMaxAmmo();
	reloading = false;
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
			gunShootSoundCurVolume = 0.0f;
		}
	}
	if (stripLaser)
	{
		stripLaser->stop();
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
	if (currentAmmo < buffedMaxAmmo && mapper.isKeyPressed(GameKey::Reload) && ultimate == false)
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

			if (ultimate == false && reloadEndSound.isValid() && !bloodworks->IsGUIHidden() )
			{
				bloodworks->addGameSound(reloadEndSound.play());
			}
		}

		if (remainingReload < 0.0)
		{
			reloading = false;
			if (ultimate == false)
			{
				currentAmmo = buffedMaxAmmo;
			}

			if (onReloadEndCall)
			{
				onReloadEndCall(this);
			}
		}
	}

	if (onTickCall)
	{
		onTickCall(this);
	}

	if (gunShootSoundContinuous && gunShootSound.isValid())
	{
		if (laser)
		{
			playGunShootSound = laser->isVisible();
		}
		if (playGunShootSound)
		{
			if (gunShootSoundFadein <= 0.0f)
			{
				gunShootSoundCurVolume = gunShootSound.getVolume();
			}
			else
			{
				gunShootSoundCurVolume += dt / gunShootSoundFadein;
			}
			gunShootSoundCurVolume = min(gunShootSoundCurVolume, gunShootSound.getVolume());
			if (gunShootSoundHandle.isValid() == false)
			{
				gunShootSoundHandle = gunShootSound.play();
				gunShootSoundHandle.setLooped(true);
				gunShootSoundHandle.setVolume(gunShootSoundCurVolume);
				bloodworks->addGameSound(gunShootSoundHandle);
			}
			else
			{
				gunShootSoundHandle.setVolume(gunShootSoundCurVolume);
			}
		}
		else if (playGunShootSound == false)
		{
			if (gunShootSoundHandle.isValid())
			{
				if (gunShootSoundFadeout <= 0.0f)
				{
					gunShootSoundCurVolume = -1.0f;
				}
				else
				{
					gunShootSoundCurVolume -= dt / gunShootSoundFadeout;
				}

				if (gunShootSoundCurVolume < 0.0f)
				{
					gunShootSoundCurVolume = 0.0f;
					gunShootSoundHandle.setVolume(gunShootSoundCurVolume);
					gunShootSoundHandle.stop();
					gunShootSoundHandle.clear();
				}
				else
				{
					gunShootSoundHandle.setVolume(gunShootSoundCurVolume);
				}
			}

		}
	}

	if (stripLaser)
	{
		Vec2 pos = bloodworks->getPlayer()->getGunPos();
		Vec2 speed = bloodworks->getPlayer()->getAimDir() * bulletSpeed;

		stripLaser->tick(pos, speed);
	}
}

float Gun::getMaxCrosshairDistance() const
{
	return crosshairDistance;
}

float Gun::getSpreadAngle() const
{
	return spreadAngle;
}

int Gun::getId() const
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
	if (laser)
	{
		laser->setPositionAndAngle(pos, angle);
	}
}

void Gun::reset()
{
	data = lua.create_table();
	id = bloodworks->getUniqueId();
	playGunShootSound = false;
	if (gunShootSoundHandle.isValid())
	{
		gunShootSoundCurVolume = 0.0f;
		gunShootSoundHandle.setVolume(gunShootSoundCurVolume);
		gunShootSoundHandle.stop();
		gunShootSoundHandle.clear();
	}
	spreadAngle = 0.0f;

	currentAmmo = maxAmmo;
	reloading = false;

	scriptTable["init"](this);

	if (laser)
	{
		laser->setVisible(false);
	}
	if (stripLaser)
	{
		stripLaser->clear();
	}
}

void Gun::onBulletHit(Bullet *bullet, Monster* monster)
{
	if (onBulletHitCall)
	{
		onBulletHitCall(this, bullet, monster);
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
	if (ultimate)
	{
		currentAmmo--;
		if (currentAmmo == 0)
		{
			bloodworks->getPlayer()->setSecondaryGun(nullptr);
			return;
		}
		else
		{
			reload();
		}
	}
	else
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
}

void Gun::reload()
{
	if (reloading == false)
	{
		bloodworks->onGunReloaded(this);
		reloading = true;
		reloadEnding = false;
		remainingReload = reloadTime;
		if (ultimate == false && reloadBeginSound.isValid() && !bloodworks->IsGUIHidden())
		{
			bloodworks->addGameSound(reloadBeginSound.play());
		}

		if (onReloadStartCall)
		{
			onReloadStartCall(this);
		}
	}
}

int Gun::getMaxAmmo() const
{
	if (bloodworks->IsGUIHidden())
	{
		return 1000;
	}
	return ultimate ? maxAmmo : bloodworks->getPlayer()->getBuffedClipSize(maxAmmo);
}

float Gun::getSpawnChance()
{
	float chance = spawnChance;
	if (dynamicSpawnChanceCall)
	{
		float mul = dynamicSpawnChanceCall(this);
		chance *= mul;
	}

	if (bloodworks->getPlayer()->getGun() == this || bloodworks->getPlayer()->getSecondaryGun() == this)
	{
		chance *= 0.1f;
	}
	return chance;
}

float Gun::getBulletLifeTime() const
{
	return bulletLifeTime;
}

const std::string& Gun::getScriptName() const
{
	return scriptName;
}

int Gun::onPlayerDamaged(int damage, float dir, sol::table& params)
{
	if (onPlayerDamagedCall)
	{
		return onPlayerDamagedCall(this, damage, dir, params);
	}
	return damage;
}

bool Gun::onBulletTick(Bullet* bullet)
{
	if (onBulletTickCall)
	{
		return onBulletTickCall(this, bullet);
	}
	return false;
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
	SAFE_DELETE(stripLaser);
}

Bullet* Gun::addBullet()
{
	Vec2 dir = bloodworks->getPlayer()->getAimDir();
	Bullet *bullet = new Bullet(bloodworks, this);
	Player *player = bloodworks->getPlayer();
	bullet->pos = player->getGunPos();
	bullet->moveSpeed = bulletSpeed;
	bullet->moveAngle = player->getAimDir().toAngle() + randFloat(-spreadAngle, spreadAngle) * player->getBulletSpreadMultiplier();
	bullet->radius = bulletRadius;
	bullet->damage = randInt(damage.x, damage.y);
	if (bulletLevelModifier)
	{
		bullet->modifyDrawLevel(bulletLevelModifier);
	}
	if (bulletTexture != nullptr)
	{
		cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, bulletTexturePath, "resources/default");
		renderable->setWorldMatrix(Mat3::scaleMatrix(bulletSize).translateBy(bulletMeshShift));
		bullet->addRenderable(renderable);
	}
	bloodworks->getBulletController()->addBullet(bullet);

	for (auto t : newBulletData)
	{
		bullet->data[t.first] = t.second;
	}

	bloodworks->onAddedGunBullet(this, bullet);

	if (gunShootSound.isValid() && lastShootSoundTime + maxSoundPlayInterval < timer.getTime() && gunShootSoundContinuous == false)
	{
		lastShootSoundTime = timer.getTime();
		bloodworks->addGameSound(gunShootSound.play());
	}

	return bullet;
}
