#include "cAnimatedRenderable.h"
#include "cShader.h"
#include "cTexture.h"

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

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(const std::string& name, nlohmann::json &animData)
{
	bool looped = false;
	if (animData.count("looped"))
	{
		looped = animData["looped"].get<bool>();
	}

	cAnimatedTexturedQuadRenderable::AnimationData animation(name, looped);

	float frameDuration = 0.1f;

	std::string artFolder = "";
	if (animData.count("baseFolder"))
	{
		artFolder = animData["baseFolder"].get<std::string>();
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

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(nlohmann::json::iterator& it)
{
	return getAnimationData(it.key(), it.value());
}

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(nlohmann::json &j)
{
	return getAnimationData(j["name"].get<std::string>(), j);
}

cAnimatedTexturedQuadRenderable::AnimationData getAnimationData(const std::string& file)
{
	std::string jsonFile;
	textFileRead(file, jsonFile);
	nlohmann::json j = nlohmann::json::parse(jsonFile.c_str());

	return getAnimationData(j);
}
