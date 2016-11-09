#include "Gun.h"
#include "Bloodworks.h"
#include "cGlobals.h"
#include "Player.h"
#include "Bullet.h"
#include "json.h"	

using json = nlohmann::json;
int Gun::nextGunId = 0;

void Gun::init(Bloodworks *bloodworks, const char *gunData)
{
	this->bloodworks = bloodworks;

	std::string jsonFile;
	textFileRead(gunData, jsonFile);
	json j = json::parse(jsonFile.c_str());

	name = j["name"].get<std::string>();
	artFolder = j["artFolder"].get<std::string>();
	fixFolderPath(artFolder);

	bulletTexturePath = artFolder + j["bulletTexture"].get<std::string>();
	bulletTexture = resources.getTexture(bulletTexturePath.c_str());
	bulletSize.w = j["bulletSize"].at(0).get<float>();
	bulletSize.h = j["bulletSize"].at(0).get<float>();
	bulletRadius = j["bulletRadius"].get<float>();
	bulletSpeed = j["bulletSpeed"].get<float>();

	damage = IntVec2(j["bulletDamage"].at(0).get<int>(), j["bulletDamage"].at(1).get<int>());

	std::string scriptFilePath = j["scriptFile"].get<std::string>();
	fixFilePath(scriptFilePath);
	scriptName = j["scriptName"].get<std::string>();
	scriptTable = lua[j["scriptName"].get<std::string>()] = lua.create_table();

	gunId = nextGunId++;

	auto guns = lua["guns"];
	luaGun = guns[gunId] = lua.create_table_with("spreadAngle", 0, "maxCrossHairRange", 50);
	lua.script_file(scriptFilePath);
	scriptTable["init"](gunId);

	spreadAngle = luaGun["spreadAngle"];
	crosshairDistance = luaGun["maxCrosshairRange"];
}

void Gun::stop()
{

}

void Gun::start()
{
	lua.set_function("addBullet",
		[&]() -> int
	{
		return addBullet();
	});
}

void Gun::tick(float dt)
{
	lua["leftMousePressed"] = input.isKeyPressed(mouse_button_left);
	lua["leftMouseDown"] = input.isKeyDown(mouse_button_left);
	lua["leftMousePressed"] = input.isKeyReleased(mouse_button_left);

	lua["middleMousePressed"] = input.isKeyPressed(mouse_button_middle);
	lua["middleMouseDown"] = input.isKeyDown(mouse_button_middle);
	lua["middleMousePressed"] = input.isKeyReleased(mouse_button_middle);

	lua["rightMousePressed"] = input.isKeyPressed(mouse_button_right);
	lua["rightMouseDown"] = input.isKeyDown(mouse_button_right);
	lua["rightMousePressed"] = input.isKeyReleased(mouse_button_right);

	scriptTable["onTick"](gunId);

	spreadAngle = luaGun["spreadAngle"];
	crosshairDistance = luaGun["maxCrosshairRange"];
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
	return gunId;
}

sol::table& Gun::getScriptTable()
{
	return scriptTable;
}

Gun::~Gun()
{
	bulletTexture = nullptr;
}

int Gun::addBullet()
{
	Vec2 dir = bloodworks->getPlayer()->getAimDir();
	Bullet *bullet = new Bullet(bloodworks, this);
	Player *player = bloodworks->getPlayer();
	bullet->pos = player->getPos() + player->getAimDir() * 20;
	bullet->speed = player->getAimDir() * bulletSpeed;
	bullet->speed *= Mat2::rotation(randFloat(-spreadAngle, spreadAngle));
	bullet->rotation = bullet->speed.toAngle();
	bullet->radius = bulletRadius;
	bullet->damage = randInt(damage.x, damage.y);
	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, bulletTexturePath.c_str(), "resources/default");
	renderable->setSize(bulletSize);
	bullet->addRenderable(renderable);
	bullet->init();
	bloodworks->getBulletController()->addBullet(bullet);

	return bullet->getId();
}
