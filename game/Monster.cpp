#include "Monster.h"
#include "Bloodworks.h"
#include "cAnimatedRenderable.h"
#include "cGlobals.h"
#include "cTools.h"
#include "json.h"
#include "cFont.h"

using json = nlohmann::json;


sol::table script;

int Monster::nextId = 0;

Monster::Monster(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	index = nextId++;
}

void Monster::init(const MonsterTemplate& monsterTemplate)
{
	name = monsterTemplate.name;

	size = monsterTemplate.size;
	textureShift = monsterTemplate.textureShift;
	hitPoint = monsterTemplate.hitPoint;


	renderable = new cAnimatedTexturedQuadRenderable(bloodworks, "resources/default");
	renderable->addAnimation(monsterTemplate.animationData);
	bloodworks->addRenderable(renderable);

	if (monsterTemplate.defaultAnimation.size())
	{
		renderable->setDefaultAnimation(renderable->getAnimationIndex(monsterTemplate.defaultAnimation));
	}

	healthRenderable = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), "", 10);
	bloodworks->addRenderable(healthRenderable);

	renderable->playAnimation("walk", randFloat());

	moveAngle = randFloat(-pi, pi);
	moveSpeed = 0.0f;


	auto monsters = lua["monsters"];
	luaMonster = monsters[index] = lua.create_table_with("x", 0, "y", 0, "moveAngle", 0, "moveSpeed", 0);


	scriptTable = monsterTemplate.scriptTable;
	setMonsterData();
	lua["monsterId"] = index;
	scriptTable["init"]();
	bulletRadius = monsterTemplate.bulletRadius;
	collisionRadius = monsterTemplate.collisionRadius;
	if (luaMonster["scale"])
	{
		float scale = luaMonster["scale"].get<float>();
		size = size * scale;
		textureShift = textureShift * scale;
		bulletRadius *= scale;
		collisionRadius *= scale;
	}

	healthRenderable->setPosition(position + Vec2(0.0f, bulletRadius + 10.0f));
	getMonsterData();

	hitPoint *= randFloat();
}

Monster::~Monster()
{
	SAFE_DELETE(renderable);
	SAFE_DELETE(healthRenderable);
}

void Monster::tick(float dt)
{
	lua["monsterId"] = index;
	scriptTable["onTick"](dt);

	std::vector<int> toTrigger;
	for (int i = (int)timers.size() - 1; i >= 0; i--)
	{
		if (timers[i].time < timer.getTime())
		{
			toTrigger.push_back(i);
		}
	}

	for (auto& i : toTrigger)
	{
		Timer t = timers[i];
		timers[i] = timers[(int)timers.size() - 1];
		timers.resize(timers.size() - 1);
		scriptTable[t.func](t.args);
	}

	getMonsterData();
	if (moveSpeed > 0.0f)
	{
		position += Vec2::fromAngle(moveAngle) * moveSpeed * dt;
		setMonsterData();
	}

	std::stringstream ss;
	ss << (int)hitPoint;

	healthRenderable->setText(ss.str());
	healthRenderable->setPosition(position + Vec2(-15 + (hitPoint<100 ? 5.0f : 0.0f) + (hitPoint<10 ? 5.0f : 0.0f), bulletRadius + 10.0f));
	Mat3 mat = Mat3::identity();
	mat.scaleBy(size);
	mat.translateBy(textureShift);
	mat.rotateBy(-pi_d2 - moveAngle);
	mat.translateBy(position);
	renderable->setWorldMatrix(mat);

	//debugRenderer.addLine(position, position + Vec2::fromAngle(moveAngle) * 50);
}


void Monster::addTimer(float timeToTrigger, const std::string& func, sol::table args, bool looped /*= false*/)
{
	Timer t;
	t.time = timeToTrigger + timer.getTime();
	t.func = func;
	t.args = args;
	t.looped = looped;
	timers.push_back(t);
}

void Monster::playAnimation(const std::string& anim)
{
	renderable->playAnimation(anim);
}

void Monster::doDamage(int damage)
{
	hitPoint -= damage;
	if (hitPoint <= 1.0f)
	{
		hitPoint = 100;
		lua["monsterId"] = index;
		scriptTable["init"]();
	}
}

void Monster::setMonsterData()
{
	luaMonster["x"] = position.x;
	luaMonster["y"] = position.y;
	luaMonster["moveAngle"] = moveAngle;
	luaMonster["moveSpeed"] = moveSpeed;
}

void Monster::getMonsterData()
{
	position.x = luaMonster["x"];
	position.y = luaMonster["y"];
	moveAngle = luaMonster["moveAngle"];
	moveSpeed = luaMonster["moveSpeed"];
}

MonsterTemplate::MonsterTemplate(const std::string& monsterData)
{
	std::string jsonFile;
	textFileRead(monsterData.c_str(), jsonFile);
	json j = json::parse(jsonFile.c_str());

	name = j["name"].get<std::string>();
	size = Vec2(j["size"].at(0).get<float>(), j["size"].at(1).get<float>());
	textureShift = Vec2(j["textureShift"].at(0).get<float>(), j["textureShift"].at(1).get<float>());
	hitPoint = j["hitPoint"].get<float>();
	bulletRadius = j["bulletRadius"].get<float>();
	collisionRadius = j["collisionRadius"].get<float>();

	std::string monsterPath = "resources/monster/";
	std::string artFolder = j["artFolder"];
	artFolder = monsterPath + artFolder + "/";


	auto& animations = j["animations"];

	for (json::iterator it = animations.begin(); it != animations.end(); ++it)
	{
		auto& animData = it.value();
		bool looped = false;
		if (animData.count("looped"))
		{
			looped = animData["looped"].get<bool>();
		}

		if (animData.count("default"))
		{
			if (animData["default"].get<bool>())
			{
				defaultAnimation = it.key();
			}
		}
		cAnimatedTexturedQuadRenderable::AnimationData animation(it.key(), looped);

		float frameDuration = 0.1f;

		if (animData.count("frameDuration"))
		{
			frameDuration = animData["frameDuration"].get<float>();
		}

		for (auto& frame : animData["frames"])
		{
			if (frame.is_string())
			{
				animation.addFrame(artFolder + frame.get<std::string>(), frameDuration);
			}
			else
			{
				float duration = frame["frameDuration"].get<float>();
				animation.addFrame(artFolder + frame["frame"].get<std::string>(), duration);
			}
		}

		animationData.push_back(animation);
	}

	scriptTable = lua[j["scriptName"].get<std::string>()] = lua.create_table();
	script = monsterPath + j["scriptFile"].get<std::string>();
	lua.require_file(name, script);
}
