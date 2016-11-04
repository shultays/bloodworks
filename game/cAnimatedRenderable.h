#pragma once

#include "cRenderable.h"

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

	void checkAnimationTime()
	{
		while (currentAnimationTime > animations[currentAnimation].frames[currentAnimationFrame].endTime)
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
				}
			}
		}
	}

	virtual void render() override;

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

		AnimationData& addFrame(const char *texturePath, float duration)
		{

			FrameData data;
			data.texture = resources.getTexture(texturePath);
			data.duration = duration;
			data.startTime = animationDuration;
			animationDuration += duration;
			data.endTime = animationDuration;
			frames.push_back(data);
			return *this;
		}

		int getIndex() const
		{
			return index;
		}
	};

private:
	std::vector<AnimationData> animations;

public:
	cAnimatedTexturedQuadRenderable(cGame *game, const char* shaderPath) : cRenderableWithShader(game, shaderPath)
	{
		currentAnimation = 0;
		currentAnimationFrame = 0;
		currentAnimationTime = 0.0f;
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

	AnimationData& addAnimation(const char *name, bool looping = false)
	{
		AnimationData animationData;
		animationData.name = name;
		animationData.looping = looping;
		animationData.index = (int)animations.size();
		animations.push_back(animationData);
		return animations[animations.size()-1];
	}

	void playAnimation(int index, float startTime = 0.0f, int nextAnimation = -1)
	{
		currentAnimation = index;
		currentAnimationTime = startTime;
		currentAnimationFrame = 0; //todo
		checkAnimationTime();
		this->nextAnimation = nextAnimation;
	}

	void playAnimation(const AnimationData& animationData, float startTime = 0.0f, int nextAnimation = -1)
	{
		playAnimation(animationData.index, startTime, nextAnimation);
	}

	void playAnimation(const char *animationName, float startTime = 0.0f, int nextAnimation = -1)
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
};