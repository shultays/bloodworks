#pragma once

#include <vector>

#include "cMat.h"
#include "cVec.h"
#include "sol.h"
#include "DirentHelper.h"

#include "cAnimatedRenderable.h"

class Bloodworks;

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
	std::string basePath;
	std::string scriptName;
	std::string scriptPath;
	sol::table scriptTable;
	sol::table scriptArgs;

	cVector<cAnimatedTexturedQuadRenderable::AnimationData> animationData;

	struct BodyPartData
	{
		cTextureShr texture;
		Vec2 shift;
	};
	cVector<BodyPartData> bodyParts;
	cVector<cTextureShr> bodyPartBits;

	cVector<cSoundSampleShr> hitSounds;
	cVector<cSoundSampleShr> killSounds;

	float bitSpeed;
public:
	MonsterTemplate(){}
	MonsterTemplate(Bloodworks* bloodworks, nlohmann::json& j, const DirentHelper::File& file);

	~MonsterTemplate();

	const std::string& getName() const
	{
		return name;
	}
	const std::string& getScriptName() const;
};
