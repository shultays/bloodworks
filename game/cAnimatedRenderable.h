#pragma once

#include "cRenderableWithShader.h"
#include "json.h"
#include "DirentHelper.h"

class cAnimationTemplate;

class cAnimatedTexturedQuadRenderable : public cRenderableWithShader
{
	struct FrameData
	{
		friend class cAnimatedTexturedQuadRenderable;
		cTextureShr texture;
		float duration;
		float startTime;
		float endTime;
	};

	int nextAnimation;
	int currentAnimation;

	int currentAnimationFrame;
	float currentAnimationTime;
	int defaultAnimation;

	float speedMultiplier;

	void checkAnimationTime()
	{
		while (animations[currentAnimation].frames.size() > 0 && animations[currentAnimation].animationDuration > 0.0f && currentAnimationTime > animations[currentAnimation].frames[currentAnimationFrame].endTime)
		{
			currentAnimationFrame++;
			if (currentAnimationFrame >= animations[currentAnimation].frames.size())
			{
				if (animations[currentAnimation].looping)
				{
					currentAnimationFrame = 0;
					currentAnimationTime -= animations[currentAnimation].animationDuration;
				}
				else
				{
					if (nextAnimation == -1)
					{
						currentAnimation = defaultAnimation;
					}
					else
					{
						currentAnimation = nextAnimation;
						nextAnimation = -1;
					}
					currentAnimationFrame = 0;
					currentAnimationTime = 0.0f;
				}
			}
		}
	}

	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop) override;

public:

	class AnimationData
	{
		friend class cAnimatedTexturedQuadRenderable;
		std::string name;
		bool looping;
		std::vector<FrameData> frames;
		int index;

		float animationDuration;
	public:
		AnimationData()
		{
			animationDuration = 0.0f;
		}

		AnimationData(const std::string& name, bool looping)
		{
			animationDuration = 0.0f;
			this->name = name;
			this->looping = looping;
		}

		AnimationData& addFrame(const std::string& texturePath, float duration);

		~AnimationData()
		{
			for (auto& frame : frames)
			{
				frame.texture = nullptr;
			}

			frames.clear();
		}

		int getIndex() const
		{
			return index;
		}

		const std::string& getName() const
		{
			return name;
		}
	};

private:
	std::vector<AnimationData> animations;

	void init()
	{
		defaultAnimation = 0;
		nextAnimation = 0;
		currentAnimation = 0;
		currentAnimationFrame = 0;
		currentAnimationTime = 0.0f;
		speedMultiplier = 1.0f;
	}
public:
	cAnimatedTexturedQuadRenderable(cGame *game, const std::string& shaderPath) : cRenderableWithShader(game, shaderPath)
	{
		init();
	}

	cAnimatedTexturedQuadRenderable(cGame *game, const cShaderShr& shader) : cRenderableWithShader(game, shader)
	{
		init();
	}

	void addAnimation(cAnimationTemplate *animationTemplate);

	void setSpeedMultiplier(float speedMultiplier)
	{
		this->speedMultiplier = speedMultiplier;
	}

	virtual ~cAnimatedTexturedQuadRenderable()
	{
		for (auto& animation : animations)
		{
			for (auto& frame : animation.frames)
			{
				frame.texture = nullptr;
			}
			animation.frames.clear();
		}
		animations.clear();
	}

	int getAnimationIndex(const std::string& animation) const
	{
		for (int i = 0; i < animations.size(); i++)
		{
			if (animations[i].name == animation)
			{
				return i;
			}
		}
		return -1;
	}

	void addAnimation(const AnimationData& animation)
	{
		animations.push_back(animation);
		animations[animations.size() - 1].index = (int) animations.size() - 1;
	}

	void addAnimation(const std::vector<AnimationData>& animations)
	{
		for (auto& animation : animations)
		{
			addAnimation(animation);
		}
	}

	const IntVec2& getTextureSize() const;

	AnimationData& addAnimation(const std::string& name, bool looping = false)
	{
		AnimationData animationData;
		animationData.name = name;
		animationData.looping = looping;
		animationData.index = (int)animations.size();
		animations.push_back(animationData);
		return animations[animations.size()-1];
	}

	void playAnimation(int index, float startPercentage = 0.0f, int nextAnimation = -1)
	{
		currentAnimation = index;
		currentAnimationTime = startPercentage * animations[index].animationDuration;
		currentAnimationFrame = 0;
		checkAnimationTime();
		this->nextAnimation = nextAnimation;
	}

	void playAnimation(const AnimationData& animationData, float startTime = 0.0f, int nextAnimation = -1)
	{
		playAnimation(animationData.index, startTime, nextAnimation);
	}

	void playAnimationWithName(const std::string& animationName, float startTime = 0.0f, int nextAnimation = -1)
	{

	}

	void playAnimation(const std::string& animationName, float startTime = 0.0f, int nextAnimation = -1)
	{
		for (auto& animation : animations)
		{
			if (animation.name == animationName)
			{
				playAnimation(animation.index, startTime, nextAnimation);
				return;
			}
		}
	}

	void setNextAnimation(int nextAnimation)
	{
		this->nextAnimation = nextAnimation;
	}

	void setDefaultAnimation(int index)
	{
		defaultAnimation = index;
	}

	virtual void setColor(const Vec4& color) override
	{
		this->color = color;
	}
};



cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(const std::string& name, nlohmann::json& animData, const DirentHelper::File& file);

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(nlohmann::json& j, const DirentHelper::File& file);

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(nlohmann::json::iterator& it, const DirentHelper::File& file);

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(const DirentHelper::File& file);
