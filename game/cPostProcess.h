#pragma once

#include "cRenderableWithShader.h"

class cPostProcess : public cUniformDataWithShader
{
	friend class cGame;
	cGame *game;
	bool enabled;
	int shaderAmountIndex;
public:
	cPostProcess()
	{
		game = nullptr;
	}
	~cPostProcess();

	void init(cGame* game, cShaderShr shader, int level = 10000);

	void setShaderWeight(float amount)
	{
		setUniform(shaderAmountIndex, amount);
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