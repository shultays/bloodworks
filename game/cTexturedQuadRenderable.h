#pragma once

#include "cRenderableWithShader.h"
#include "cResources.h"

class cTexturedQuadRenderable : public cRenderableWithShader
{
protected:
	cTextureShr texture[4];

public:
	cTexturedQuadRenderable(cGame *game, const std::string& texturePath, const std::string& shaderPath);

	virtual ~cTexturedQuadRenderable()
	{
		for (int i = 0; i < 4; i++)
		{
			texture[i] = nullptr;
		}
	}

	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;

	void setTexture(const std::string& texturePath)
	{
		setTexture(0, texturePath);
	}

	void setTexture(int i, const std::string& texturePath);

	cTextureShr& getTexture()
	{
		return getTexture(0);
	}

	cTextureShr& cTexturedQuadRenderable::getTexture(int i)
	{
		return texture[i];
	}
};