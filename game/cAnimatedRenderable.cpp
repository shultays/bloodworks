#include "cAnimatedRenderable.h"
#include "cShader.h"
#include "cTexture.h"
#include "cGlobals.h"
#include "DirentHelper.h"
#include "cAnimationTemplate.h"

void cAnimatedTexturedQuadRenderable::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	if (animations[currentAnimation].frames.size() > 0)
	{
		cRenderableWithShader::render(isIdentity, mat, crop);
		currentAnimationTime += timer.getRender_dt() * speedMultiplier;
		checkAnimationTime();

		if (animations[currentAnimation].frames.size() > 0)
		{

			glBindBuffer(GL_ARRAY_BUFFER, quad);

			shader->bindPosition(sizeof(float) * 8, 0);
			shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
			shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

			shader->setColor(color);


			cTextureShr texture = animations[currentAnimation].frames[currentAnimationFrame].texture;

			texture->bindTexture();
			shader->setWorldMatrix(isIdentity ? worldMatrix : worldMatrix * mat);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glDisableVertexAttribArray(0);

			glDisable(GL_TEXTURE_2D);
		}
	}
}

void cAnimatedTexturedQuadRenderable::addAnimation(cAnimationTemplate *animationTemplate)
{
	addAnimation(animationTemplate->getAnimations());
}

const IntVec2& cAnimatedTexturedQuadRenderable::getTextureSize() const
{
	return animations[0].frames[0].texture->getDimensions();
}

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(const std::string& name, nlohmann::json& animData, const DirentHelper::File& file)
{
	bool looped = false;
	if (animData.count("looped"))
	{
		looped = animData["looped"].get<bool>();
	}

	cAnimatedTexturedQuadRenderable::AnimationData animation(name, looped);

	float frameDuration = 0.1f;

	std::string artFolder = file.folder;
	if (animData.count("baseFolder"))
	{
		artFolder += animData["baseFolder"].get<std::string>();
	}
	fixFolderPath(artFolder);
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
		else if (frame.is_array())
		{
			animation.addFrame(artFolder + frame[0].get<std::string>(), frame[1].get<float>());
		}
		else
		{
			float duration = frame["frameDuration"].get<float>();
			animation.addFrame(artFolder + frame["frame"].get<std::string>(), duration);
		}
	}

	return animation;
}

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(nlohmann::json::iterator& it, const DirentHelper::File& file)
{
	return getAnimationData(it.key(), it.value(), file);
}

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(nlohmann::json& j, const DirentHelper::File& file)
{
	return getAnimationData(j["name"].get<std::string>(), j, file);
}

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(const DirentHelper::File& file)
{
	std::string jsonFile;
	textFileRead(file.folder + file.file, jsonFile);
	nlohmann::json j = nlohmann::json::parse(jsonFile.c_str());

	return getAnimationData(j, file);
}

cAnimatedTexturedQuadRenderable::AnimationData& cAnimatedTexturedQuadRenderable::AnimationData::addFrame(const std::string& texturePath, float duration)
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
