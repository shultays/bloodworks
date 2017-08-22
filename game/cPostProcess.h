#pragma once

#include "cRenderableWithShader.h"

class cPostProcess : public cUniformDataWithShader
{
	friend class cGame;
	cGame *game;
	bool enabled;
	int shaderAmountIndex;
	int screenSizeIndex;
	int timeIndex;
	int zoomIndex;
	float shaderWeight;
public:
	cPostProcess()
	{
		game = nullptr;
		shaderWeight = 1.0f;
	}
	~cPostProcess();

	void init(cGame* game, cShaderShr shader, int level = 10000);

	void setShaderWeight(float amount)
	{
		shaderWeight = amount;
		setUniform(shaderAmountIndex, amount);
	}

	float getShaderWeight() const
	{
		return shaderWeight;
	}

	cShaderShr& getShader()
	{
		return shader;
	}

	void bind();

	bool isEnabled() const
	{
		return enabled;
	}

	void setEnabled(bool enabled)
	{
		if (this->enabled != enabled)
		{
			this->enabled = enabled;
		}
	}
};