#pragma once

#include <vector>

#include "cMat.h"
#include "cVec.h"
#include "sol.h"

#include "cAnimatedRenderable.h"

class MonsterTemplate
{
	friend class BloodworksLuaWorld;
	friend class Monster;
	Vec2 size;
	Vec2 textureShift;
	std::string name;
	int hitPoint;
	int experience;
	int score;
	float collisionRadius;
	float bulletRadius;
	bool hasBlood;
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
	MonsterTemplate(nlohmann::json& j);

	~MonsterTemplate();

	const std::string& getName() const
	{
		return name;
	}
};
