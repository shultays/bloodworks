#pragma once

#include "DirentHelper.h"
#include "json.h"
#include "cAnimatedRenderable.h"


class cAnimationTemplate 
{
	std::string name;
	cShaderShr shader;
	Vec2 size;
	cVector<cAnimatedTexturedQuadRenderable::AnimationData> animationData;
public:
	cAnimationTemplate(nlohmann::json& j, const DirentHelper::File& file)
	{
		name = j["name"].get<std::string>();
		size = Vec2(j["size"].at(0).get<float>(), j["size"].at(1).get<float>());

		std::string shaderPath = "resources/default";
		if (j.count("shader"))
		{
			shaderPath = j["shader"].get<std::string>();
		}
		shader = resources.getShader(shaderPath);
		auto& animations = j["animations"];

		for (nlohmann::json::iterator it = animations.begin(); it != animations.end(); ++it)
		{
			cAnimatedTexturedQuadRenderable::AnimationData data = getAnimationData(it, file);
			animationData.push_back(data);
		}
	}

	const std::string& getName() const
	{
		return name;
	}

	const Vec2& getSize() const
	{
		return size;
	}

	const cVector<cAnimatedTexturedQuadRenderable::AnimationData>& getAnimations() const
	{
		return animationData;
	}


	const cShaderShr& getShader() const
	{
		return shader;
	}
};