#pragma once

#include <vector>

#include "cMat.h"
#include "cVec.h"
#include "cLuaWorld.h"
#include "cAnimatedRenderable.h"
#include "json.h"

using json = nlohmann::json;

class MonsterTemplate
{
	friend class Monster;
	Vec2 size;
	Vec2 textureShift;
	std::string name;
	int hitPoint;
	float collisionRadius;
	float bulletRadius;
	std::string scriptPath;
	sol::table scriptTable;

	std::string defaultAnimation;
	std::vector<cAnimatedTexturedQuadRenderable::AnimationData> animationData;
public:
	MonsterTemplate(){}
	MonsterTemplate(const std::string& monsterData)
	{
		init(monsterData);
	}
	void init(const std::string& monsterData)
	{
		std::string jsonFile;
		textFileRead(monsterData.c_str(), jsonFile);
		json j = json::parse(jsonFile.c_str());

		name = j["name"].get<std::string>();
		size = Vec2(j["size"].at(0).get<float>(), j["size"].at(1).get<float>());
		textureShift = Vec2(j["textureShift"].at(0).get<float>(), j["textureShift"].at(1).get<float>());
		hitPoint = j["hitPoint"].get<int>();
		bulletRadius = j["bulletRadius"].get<float>();
		collisionRadius = j["collisionRadius"].get<float>();

		std::string artFolder = j["artFolder"].get<std::string>();
		fixFolderPath(artFolder);
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
		scriptPath = j["scriptFile"].get<std::string>();
		lua.script_file(scriptPath);
	}

};
