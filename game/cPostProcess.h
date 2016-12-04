#pragma once

#include "cGame.h"

class cPostProcess
{
	friend class cGame;
	cShaderShr shader;
	cGame *game;
	bool enabled;
	int shaderAmountIndex;
	float shaderAmount;
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
		shader = nullptr;
	}

	void init(cGame* game, cShaderShr shader, int level = 10000)
	{
		enabled = false;
		this->shader = shader;
		this->game = game;
		game->addPostProcess(this, level);
		shaderAmountIndex = shader->addUniform("uShaderAmount", TypeFloat).index;
		shaderAmount = 0.0f;
	}
	void setShaderAmount(float amount)
	{
		this->shaderAmount = amount;
	}

	cShaderShr& getShader()
	{
		return shader;
	}

	void bind()
	{
		shader->begin();

		shader->setUniform(shaderAmountIndex, shaderAmount);
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