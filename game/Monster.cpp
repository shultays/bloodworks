#include "Monster.h"
#include "Bloodworks.h"
#include "cAnimatedRenderable.h"
#include "cGlobals.h"
#include "cTools.h"
#include "json.h"

using json = nlohmann::json;


sol::table script;

int Monster::nextId = 0;

Monster::Monster(Bloodworks *bloodworks, const MonsterTemplate& monsterTemplate)
{
	this->bloodworks = bloodworks;

	index = nextId++;

	name = monsterTemplate.name;

	size = monsterTemplate.size;
	textureShift = monsterTemplate.textureShift;
	hitPoint = monsterTemplate.hitPoint;

	renderable = new cAnimatedTexturedQuadRenderable(bloodworks, "resources/default");
	renderable->addAnimation(monsterTemplate.animationData);
	bloodworks->addRenderable(renderable);

	renderable->playAnimation("walk", randFloat());

	moveAngle = randFloat(-pi, pi);
	moveSpeed = 0.0f;


	auto monsters = lua["monsters"];
	luaMonster = monsters[index] = lua.create_table_with("x", 0, "y", 0, "moveAngle", 0, "moveSpeed", 0);


	scriptTable = monsterTemplate.scriptTable;
	setMonsterData();
	lua["monsterId"] = index;
	scriptTable["init"]();
	if (luaMonster["scale"])
	{
		float scale = luaMonster["scale"].get<float>();
		size = size * scale;
		textureShift = textureShift * scale;
	}
	getMonsterData();
}

Monster::~Monster()
{
	SAFE_DELETE(renderable);
}

void Monster::tick(float dt)
{
	if (input.isKeyPressed(key_1))
	{
		renderable->playAnimation("stand");
	}
	if (input.isKeyPressed(key_2))
	{
		renderable->playAnimation("walk");
	}
	if (input.isKeyPressed(key_3))
	{
		renderable->playAnimation("attack", 0.0f, 1);
	}

	lua["monsterId"] = index;
	scriptTable["onTick"](dt);

	getMonsterData();
	if (moveSpeed > 0.0f)
	{
		position += Vec2::fromAngle(moveAngle) * moveSpeed * dt;
		setMonsterData();
	}

	Mat3 mat = Mat3::identity();
	mat.scaleBy(size);
	mat.translateBy(textureShift);
	mat.rotateBy(-pi_d2 - moveAngle);
	mat.translateBy(position);
	renderable->setWorldMatrix(mat);

	debugRenderer.addLine(position, position + Vec2::fromAngle(moveAngle) * 50);
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
	hitPoint = j["hitpoint"].get<float>();

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
