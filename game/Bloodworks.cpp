#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Bullet.h"
#include "Monster.h"
#include "Gun.h"
#include "Bonus.h"
#include "BloodRenderable.h"
#include "cFont.h"

#include <sstream>

#include "cParticle.h"

cParticle *p;

void Bloodworks::init()
{
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

		"normalize", [](Vec2& a) { a.normalize(); },
		"normalized", [](const Vec2& a) { return a.normalized(); },

		"distance", [](const Vec2& a, const Vec2& b) { return a.distance(b); },
		"distanceSquared", [](const Vec2& a, const Vec2& b) { return a.distanceSquared(b); },

		"length", [](const Vec2& a) { return a.length(); },
		"lengthSquared", [](const Vec2& a) { return a.lengthSquared(); },

		"sideVec", &Vec2::sideVec,

		"dot", [](const Vec2& a, const Vec2& b) { return a.dot(b); }
	);



	lua.set_function("approachAngle",
		[&](float angle, float angleToApproach, float maxRotation) -> float
	{
		return approachAngle(angle, angleToApproach, maxRotation);
	});


	lua.set_function("addLine",
		[&](const Vec2& pos0, const Vec2& pos1)
	{
		debugRenderer.addLine(pos0, pos1);
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

	bg = new cTexturedQuadRenderable(this, "resources/bg.png", "resources/default");
	bg->setWorldMatrix(Mat3::scaleMatrix(512.0f));
	addRenderable(bg, 0);

	input.hideMouse();

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;

	player = new Player(this);
	Gun *gun = new Gun();
	gun->init(this, "resources/basicgun/data.json");
	guns.push_back(gun);
	player->setGun(gun);

	gun = new Gun();
	gun->init(this, "resources/machinegun/data.json");
	guns.push_back(gun);

	Bonus *bonus = new Bonus("resources/bonuses/circle_fire/data.json");
	bonuses.push_back(bonus);

	bonus = new Bonus("resources/bonuses/homing/data.json");
	bonuses.push_back(bonus);

	monsterController.init(this);
	bulletController.init(this);

	missionController.init(this);
	missionController.loadMissionController("resources/missions/survival/data.json");

	bloodRenderable = new BloodRenderable(this);
	bloodRenderable->init();
	addRenderable(bloodRenderable, 1);

	p = new cParticle(this);
	addRenderable(p, 1000);
}

Bloodworks::~Bloodworks()
{
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
	monsterController.clear();
	bulletController.clear();
	missionController.clear();
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
	renderable->setAlignment(cTextRenderable::center);
	renderable->setWorldMatrix(Mat3::translationMatrix(position - Vec2(0.0f, 5.0f)));
	addRenderable(renderable, 400);

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
	renderable->setAlignment(cTextRenderable::center);
	renderable->setWorldMatrix(Mat3::translationMatrix(position - Vec2(0.0f, 5.0f)));
	addRenderable(renderable, 400);

	drops.push_back(drop);
}

const Mat3& Bloodworks::getViewMatrix() const
{
	return worldViewMatrix;
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

void Bloodworks::tick(float dt)
{
	tickCount++;
	if (timer.getTime() - lastSetTickTime > 1.0f)
	{
		lastSetTickTime += 1.0f;
		std::stringstream ss;
		ss << "FPS " << tickCount;
		debugRenderer.addScreenText(0, ss.str(), 5.0f, 5.0f, FLT_MAX);

		tickCount = 0;
	}

	/*
	static bool start = false;
	if (input.isKeyPressed(key_space))
	{
		start = !start;
	}
	if (start == false) return;
	*/
	
	lua["dt"] = dt;
	lua["time"] = timer.getTime();

	bloodRenderable->tick();

	missionController.tick(dt);

	player->tick(dt);

	monsterController.tick(dt);
	bulletController.tick(dt);

	if (input.isKeyPressed(mouse_button_right))
	{
		p->addParticle(player->getPos());
	}

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
}

void Bloodworks::render()
{
	renderCount++;
	if (timer.getTime() - lastSetRenderTime > 1.0f)
	{
		lastSetRenderTime += 1.0f;
		std::stringstream ss;
		ss << "Render " << renderCount;
		debugRenderer.addScreenText(1, ss.str(), 5.0f, 35.0f, FLT_MAX);

		renderCount = 0;
	}
}
