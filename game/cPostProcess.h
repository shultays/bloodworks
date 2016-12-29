#pragma once

#include "cGame.h"

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
	~cPostProcess()
	{
		if (game)
		{
			game->removePostProcess(this);
		}
	}

	void init(cGame* game, cShaderShr shader, int level = 10000)
	{
		enabled = true;
		this->shader = shader;
		this->game = game;
		game->addPostProcess(this, level);
		shaderAmountIndex = addUniformFloat("uShaderAmount", 1.0f);
	}

	void setShaderWeight(float amount)
	{
		setUniform(shaderAmountIndex, amount);
	}

	cShaderShr& getShader()
	{
		return shader;
	}

	void bind()
	{
		shader->begin();

		setShaderUniforms();
	}

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