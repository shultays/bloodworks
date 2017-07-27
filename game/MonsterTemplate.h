#pragma once

#include <vector>

#include "cMat.h"
#include "cVec.h"
#include "sol.h"
#include "DirentHelper.h"

#include "cAnimatedRenderable.h"

class MonsterTemplate
{
	friend class BloodworksLuaWorld;
	friend class Monster;
	friend class Bloodworks;
	Vec2 size;
	Vec2 textureShift;
	std::string name;
	int hitPoint;
	int experience;
	int score;
	float collisionRadius;
	float bulletRadius;
	bool hasBlood;
	std::string scriptName;
	std::string scriptPath;
	sol::table scriptTable;
	sol::table scriptArgs;

	std::vector<cAnimatedTexturedQuadRenderable::AnimationData> animationData;

	struct BodyPartData
	{
		cTextureShr texture;
		Vec2 shift;
	};
	std::vector<BodyPartData> bodyParts;
	std::vector<cTextureShr> bodyPartBits;

	std::vector<cSoundSampleShr> hitSounds;
	std::vector<cSoundSampleShr> killSounds;

	float bitSpeed;
public:
	MonsterTemplate(){}
	MonsterTemplate(nlohmann::json& j, const DirentHelper::File& file);

	~MonsterTemplate();

	const std::string& getName() const
	{
		return name;
	}
	const std::string& getScriptName() const;
};
