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

#include <sstream>

#include "cParticle.h"


int Bloodworks::nextUniqueId = 0;

void Bloodworks::init()
{
	targetGamePlaySlowdown = pauseSlowdown = gamePlaySlowdown = 1.0f;
	paused = false;

	lua.script_file("resources/helpers.lua");

	lua.new_usertype<Vec2>("Vec2",
		sol::constructors<sol::types<>, sol::types<float, float>>(),
		"x", &Vec2::x,
		"y", &Vec2::y,

		sol::meta_function::addition, [](const Vec2& a, const Vec2& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vec2& a, const Vec2& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vec2& a, const Vec2& b) { return a * b; },
		sol::meta_function::multiplication, [](const Vec2& a, float b) { return a * b; },
		sol::meta_function::division, [](const Vec2& a, const Vec2& b) { return a / b; },
		sol::meta_function::division, [](const Vec2& a, float b) { return a / b; },

		"setAngle", [](Vec2& v, float angle) { v = Vec2::fromAngle(angle); },
		"getAngle", &Vec2::toAngle,

		"normalize", [](Vec2& a) { return a.normalize(); },
		"normalized", [](const Vec2& a) { return a.normalized(); },

		"safeNormalize", [](Vec2& a) { return a.safeNormalize(); },
		"safeNormalized", [](const Vec2& a) { return a.safeNormalized(); },

		"distance", [](const Vec2& a, const Vec2& b) { return a.distance(b); },
		"distanceSquared", [](const Vec2& a, const Vec2& b) { return a.distanceSquared(b); },

		"length", [](const Vec2& a) { return a.length(); },
		"lengthSquared", [](const Vec2& a) { return a.lengthSquared(); },

		"sideVec", &Vec2::sideVec,

		"dot", [](const Vec2& a, const Vec2& b) { return a.dot(b); }
	);


	lua.new_usertype<Vec3>("Vec3",
		sol::constructors<sol::types<>, sol::types<float, float, float>>(),
		"x", &Vec3::x,
		"y", &Vec3::y,
		"z", &Vec3::z,

		sol::meta_function::addition, [](const Vec3& a, const Vec3& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vec3& a, const Vec3& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vec3& a, const Vec3& b) { return a * b; },
		sol::meta_function::multiplication, [](const Vec3& a, float b) { return a * b; },
		sol::meta_function::division, [](const Vec3& a, const Vec3& b) { return a / b; },
		sol::meta_function::division, [](const Vec3& a, float b) { return a / b; },


		"normalize", [](Vec3& a) { a.normalize(); },
		"normalized", [](const Vec3& a) { return a.normalized(); },

		"distance", [](const Vec3& a, const Vec3& b) { return a.distance(b); },
		"distanceSquared", [](const Vec3& a, const Vec3& b) { return a.distanceSquared(b); },

		"length", [](const Vec3& a) { return a.length(); },
		"lengthSquared", [](const Vec3& a) { return a.lengthSquared(); },

		"dot", [](const Vec3& a, const Vec3& b) { return a.dot(b); }
	);

	lua.new_usertype<Mat3>("Mat3",
		"fromPosition", [](const Vec2& pos) { return Mat3::translationMatrix(pos); },
		"fromPositionAndScale", [](const Vec2& pos, const Vec2& scale) { return Mat3::scaleMatrix(scale).translateBy(pos); },
		"from", [](const Vec2& pos, const Vec2& scale, float rotation) { return Mat3::scaleMatrix(scale).rotateBy(rotation).translateBy(pos); }
	);

	lua.new_usertype<cRenderable>("Renderable",
		"setLevel", &cRenderable::setLevel,
		"setAllignment", &cRenderable::setAlignment,
		"setVisible", &cRenderable::setVisible,
		"setColor", &cRenderable::setColor,
		"setWorldMatrix", &cRenderable::setWorldMatrix
		);

	lua.new_usertype<cParticle>("Particle",
		"setLevel", &cParticle::setLevel,
		"setAllignment", &cParticle::setAlignment,
		"setVisible", &cParticle::setVisible,
		"setColor", &cParticle::setColor,
		"setWorldMatrix", &cParticle::setWorldMatrix,
		"addParticle", &cParticle::addParticle
		);

	lua.new_usertype<Bullet>("Bullet",
		"index", sol::readonly(&Bullet::id),

		"position", &Bullet::pos,
		"moveSpeed", &Bullet::moveSpeed,
		"moveAngle", &Bullet::moveAngle,
		"meshRotation", &Bullet::meshRotation,

		"meshScale", &Bullet::meshScale,

		"moveDir", sol::readonly(&Bullet::moveDir),
		"moveSpeedDir", sol::readonly(&Bullet::moveSpeedDir),

		"radius", &Bullet::radius,
		"damage", &Bullet::damage,

		"diesOnHit", &Bullet::diesOnHit,
		"updateDrawable", &Bullet::updateDrawable,

		"script", &Bullet::script,

		"onHitCallback", &Bullet::onHitCallback,
		"onTickCallback", &Bullet::onTickCallback,
		"shouldHitMonsterTest", &Bullet::shouldHitMonsterTest,

		"isDead", sol::readonly(&Bullet::isDead),

		"data", &Bullet::data,

		"addRenderableTexture", &Bullet::addRenderableTexture,
		"addRenderableTextureWithSize", &Bullet::addRenderableTextureWithSize,
		"addRenderableTextureWithPosAndSize", &Bullet::addRenderableTextureWithPosAndSize,
		"addTrailParticle", &Bullet::addTrailParticle,

		"removeSelf", &Bullet::removeSelf
		);
	lua.set_function("approachAngle",
		[&](float angle, float angleToApproach, float maxRotation) -> float
	{
		return approachAngle(angle, angleToApproach, maxRotation);
	});

	lua.set_function("multiplyGameSpeed", [&](float multiplier)
	{
		multiplyGameSpeed(multiplier);
	});

	lua.set_function("addLine",
		[&](const Vec2& pos0, const Vec2& pos1)
	{
		debugRenderer.addLine(pos0, pos1);
	});

	lua.set_function("addExplosion",
		[&](const Vec2& pos, float scale, float speed, int minDamage, int maxDamage)
	{
		addExplosion(pos, scale, speed, minDamage, maxDamage);
	});


	lua["mission"] = lua.create_table();

	lua.new_usertype<Gun>("Gun",
		"index", sol::readonly(&Gun::id),
		"data", &Gun::data,

		"bulletSpeed", &Gun::bulletSpeed,
		"bulletRadius", &Gun::bulletRadius,
		"bulletSpeed", &Gun::bulletSpeed,
		"scriptTable", &Gun::scriptTable,

		"spreadAngle", &Gun::spreadAngle,
		"crosshairDistance", &Gun::crosshairDistance,

		"leftMousePressed", &Gun::leftMousePressed,
		"leftMouseDown", &Gun::leftMouseDown,
		"leftMouseReleased", &Gun::leftMouseReleased,

		"middleMousePressed", &Gun::middleMousePressed,
		"middleMouseDown", &Gun::middleMouseDown,
		"middleMouseReleased", &Gun::middleMouseReleased,
		"rightMousePressed", &Gun::rightMousePressed,
		"rightMouseDown", &Gun::rightMouseDown,
		"rightMouseReleased", &Gun::rightMouseReleased,

		"addBullet", &Gun::addBullet
	);


	mapSize = 1048.0f;
	mapBegin = -mapSize*0.5f;
	mapEnd = mapBegin + mapSize;

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

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;

	player = new Player(this);
	
	lua.script_file("resources/guns/helpers.lua");

	Gun *gun;
	gun = new Gun();
	gun->init(this, "resources/guns/basicgun/data.json");
	guns.push_back(gun);
	player->setGun(gun);

	gun = new Gun();
	gun->init(this, "resources/guns/machinegun/data.json");
	guns.push_back(gun);

	gun = new Gun();
	gun->init(this, "resources/guns/rocketlauncher/data.json");
	guns.push_back(gun);
	player->setGun(gun);

	Bonus *bonus;

	bonus = new Bonus("resources/bonuses/reflex_boost/data.json");
	bonuses.push_back(bonus);

	bonus = new Bonus("resources/bonuses/circle_fire/data.json");
	bonuses.push_back(bonus);

	bonus = new Bonus("resources/bonuses/homing/data.json");
	bonuses.push_back(bonus);

	bonus = new Bonus("resources/bonuses/explosion/data.json");
	bonuses.push_back(bonus);

	monsterController.init(this);
	bulletController.init(this);

	missionController.init(this);
	missionController.loadMissionController("resources/missions/survival/data.json");

	bloodRenderable = new BloodRenderable(this);
	bloodRenderable->init();
	addRenderable(bloodRenderable, BACKGROUND + 1);

	cParticleTemplate *particleTemplate;

	particleTemplate = new cParticleTemplate();
	particleTemplate->init("resources/particles/rocketSmoke/data.json");
	particles[particleTemplate->getName()] = particleTemplate;

	particleTemplate = new cParticleTemplate();
	particleTemplate->init("resources/particles/critical/data.json");
	particles[particleTemplate->getName()] = particleTemplate;

	particleTemplate = new cParticleTemplate();
	particleTemplate->init("resources/particles/explosionFire/data.json");
	fireParticle = particles[particleTemplate->getName()] = particleTemplate;

	explosionParticles = new cParticle(this, fireParticle, lua.create_table());
	addRenderable(explosionParticles, MONSTERS + 1);

	cameraCenterPos.setZero();

	ring = resources.getTexture("resources/particles/explosionFire/ring.png");


	Perk *perk;

	perk = new Perk();
	perk->load("resources/perks/critical/data.json");
	perks.push_back(perk);

	perk = new Perk();
	perk->load("resources/perks/no_slowdown_on_hit/data.json");
	perks.push_back(perk);

	perk = new Perk();
	perk->load("resources/perks/faster_shoot/data.json");
	perks.push_back(perk);

	perk = new Perk();
	perk->load("resources/perks/faster_bullets/data.json");
	perks.push_back(perk);

	perk = new Perk();
	perk->load("resources/perks/faster_movement/data.json");
	perks.push_back(perk);
}

Bloodworks::~Bloodworks()
{
	for (auto& particle : particles)
	{
		SAFE_DELETE(particle.second);
	}
	particles.clear();
	SAFE_DELETE(explosionParticles);

	player->setGun(nullptr);
	SAFE_DELETE(bloodRenderable);
	SAFE_DELETE(bg);
	SAFE_DELETE(player);
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

	for (auto& drop : drops)
	{
		SAFE_DELETE(drop.renderable);
	}
	drops.clear();

	for (auto& explosionData : explosions)
	{
		SAFE_DELETE(explosionData.ringRenderable);
	}
	explosions.clear();

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

	monsterController.clear();
	bulletController.clear();
	missionController.clear();

	ring = nullptr;
}

void Bloodworks::onAddedGunBullet(Gun *gun, Bullet *bullet)
{
	for (auto& perk : usedPerks)
	{
		if (perk->onAddGunBullet)
		{
			perk->onAddGunBullet(gun, bullet);
		}
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

BloodRenderable* Bloodworks::getBloodRenderable()
{
	return bloodRenderable;
}

void Bloodworks::createGun(const Vec2& position)
{
	Drop drop;
	drop.bonus = nullptr;

	do 
	{
		drop.gun = guns[randInt((int)guns.size())];
	} while (drop.gun == player->getGun());

	drop.pos = position;
	cTextRenderable *renderable;
	drop.renderable = renderable = new cTextRenderable(this, resources.getFont("resources/fontSmallData.txt"), drop.gun->getName(), 11);
	renderable->setTextAllignment(TextAlignment::center);
	renderable->setWorldMatrix(Mat3::translationMatrix(position - Vec2(0.0f, 5.0f)));
	addRenderable(renderable, OBJECT_GUI);

	drops.push_back(drop);
}

void Bloodworks::createBonus(const Vec2& position)
{
	Drop drop;
	drop.bonus = bonuses[randInt((int)bonuses.size())];
	drop.gun = nullptr;

	drop.pos = position;
	cTextRenderable *renderable;
	drop.renderable = renderable = new cTextRenderable(this, resources.getFont("resources/fontSmallData.txt"), drop.bonus->name, 11);
	renderable->setTextAllignment(TextAlignment::center);
	renderable->setWorldMatrix(Mat3::translationMatrix(position - Vec2(0.0f, 5.0f)));
	addRenderable(renderable, OBJECT_GUI);

	drops.push_back(drop);
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
	sol::table params = lua.create_table();
	float particleScale = maxScale * 0.67f;
	params["size"] = particleScale;
	params["duration"] = maxScale / scaleSpeed;
	for (int i = 0; i < 15; i++)
	{
		explosionParticles->addParticle(pos, params);
	}
	ExplosionData explosionData;
	explosionData.ringRenderable = new cTexturedQuadRenderable(this, "resources/particles/explosionFire/ring.png", "resources/default");
	explosionData.ringRenderable->setWorldMatrix(Mat3::scaleMatrix(0.0f).translateBy(pos));
	explosionData.ringRenderable->setColor(Vec4(0.0f));

	explosionData.lastDamageScale = -50.0f;
	explosionData.maxScale = maxScale;
	explosionData.scaleSpeed = scaleSpeed;
	explosionData.startTime = timer.getTime();
	explosionData.minDamage = minDamage;
	explosionData.maxDamage = maxDamage;

	explosionData.pos = pos;
	explosionData.id = getUniqueId();
	addRenderable(explosionData.ringRenderable, MONSTERS + 1);

	explosions.push_back(explosionData);
}

void Bloodworks::addDrop(const Vec2& position)	
{
	if (randBool())
	{
		createBonus(position);
	}
	else
	{
		createGun(position);
	}
}

void Bloodworks::tick()
{
	tickCount++;
	if (timer.getRealTime() - lastSetTickTime > 1.0f)
	{
		lastSetTickTime += 1.0f;
		std::stringstream ss;
		ss << "FPS " << tickCount;
		debugRenderer.addText(0, ss.str(), 5.0f, -24.0f, FLT_MAX, Vec4(1.0f), 24.0f, TextAlignment::left, RenderableAlignment::topLeft);

		tickCount = 0;
	}

	lua["dt"] = timer.getDt();
	lua["time"] = timer.getTime();

	if (input.isKeyPressed(key_2) && perks[0]->used == false)
	{
		perks[0]->use();
		usedPerks.push_back(perks[0]);
	}

	if (input.isKeyPressed(key_3))
	{
		lua[bonuses[0]->scriptName]["spawn"](player->getPos());
	}

	bloodRenderable->tick();

	missionController.tick();

	player->tick();

	if (cameraCenterPos.x > player->getPos().x + 50.0f)
	{
		cameraCenterPos.x = player->getPos().x + 50;
	}
	else if (cameraCenterPos.x < player->getPos().x - 50.0f)
	{
		cameraCenterPos.x = player->getPos().x - 50;
	}
	if (cameraCenterPos.y > player->getPos().y + 50.0f)
	{
		cameraCenterPos.y = player->getPos().y + 50;
	}
	else if (cameraCenterPos.y < player->getPos().y - 50.0f)
	{
		cameraCenterPos.y = player->getPos().y - 50;
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

	monsterController.tick();
	bulletController.tick();

	for(int i=0; i< drops.size(); i++)
	{
		auto& drop = drops[i];
		if (drop.pos.distanceSquared(player->getPos()) < 20.0f * 20.0f)
		{
			if (drop.gun)
			{
				player->setGun(drop.gun);
			}
			else
			{
				lua[drop.bonus->scriptName]["spawn"](drop.pos);
			}
			SAFE_DELETE(drop.renderable);
			drops[i] = drops[(int)drops.size() - 1];
			drops.resize((int)drops.size() - 1);
			i--;
		}
	}

	for (int i=0; i < explosions.size(); i++)
	{
		auto& explosionData = explosions[i];
		float dt = timer.getTime() - explosionData.startTime;

		float newScale = dt * explosionData.scaleSpeed;

		if (newScale > explosionData.lastDamageScale || newScale > explosionData.maxScale)
		{
			explosionData.lastDamageScale = newScale + 15.0f;
			float damageScale = min(newScale + 15.0f, explosionData.maxScale);
			std::stringstream explosionId;
			explosionId << "explosion" << explosionData.id;
			monsterController.damageMonstersInRangeWithIgnoreData(explosionData.pos, damageScale, explosionData.minDamage, explosionData.maxDamage, true, explosionId.str());
		}

		if (newScale > explosionData.maxScale)
		{
			SAFE_DELETE(explosions[i].ringRenderable);
			explosions[i] = explosions[(int)explosions.size() - 1];
			explosions.resize(explosions.size() - 1);
			i--;
			continue;
		}

		float alpha = 0.0f;
		float a0 = min(20.0f, explosionData.maxScale * 0.2f);
		float a1 = min(50.0f, explosionData.maxScale * 0.5f);
		float a2 = max(explosionData.maxScale - 50.0f, explosionData.maxScale * 0.8f, a1);

		if (newScale < a0)
		{
			alpha = 0.0f;
		}
		else if (newScale < a1)
		{
			alpha = (newScale - a0) / (a1 - a0);
		}
		else if (newScale < a2)
		{
			alpha = 1.0f;
		}
		else
		{
			alpha = (explosionData.maxScale - newScale) / (explosionData.maxScale - a2);
		}
		explosionData.ringRenderable->setWorldMatrix(Mat3::scaleMatrix(newScale + 10.0f).translateBy(explosionData.pos));
		explosionData.ringRenderable->setColor(Vec4(1.0f, 1.0f, 1.0f, alpha * 0.4f));
	}

	if (input.isKeyPressed(key_p))
	{
		paused = !paused;
	}

	bool changeSlowdown = false;
	if (paused && pauseSlowdown > 0.0f)
	{
		pauseSlowdown -= timer.realDt * 2.0f;
		if (pauseSlowdown < 0.0f)
		{
			pauseSlowdown = 0.0f;
		}
		changeSlowdown = true;
	}
	else if (!paused && pauseSlowdown < 1.0f)
	{
		pauseSlowdown += timer.realDt * 2.0f;
		if (pauseSlowdown > 1.0f)
		{
			pauseSlowdown = 1.0f;
		}
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
}

void Bloodworks::render()
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
