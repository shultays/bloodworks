#include "cPostProcess.h"
#include "cGame.h"
#include "cShader.h"

cPostProcess::~cPostProcess()
{
	if (game)
	{
		game->removePostProcess(this);
	}
}

void cPostProcess::init(cGame* game, cShaderShr shader, int level /*= 10000*/)
{
	enabled = true;
	this->shader = shader;
	this->game = game;
	game->addPostProcess(this, level);
	shaderAmountIndex = addUniformFloat("uShaderAmount", 1.0f);
}

void cPostProcess::bind()
{
	shader->begin();

	setShaderUniforms();
}
