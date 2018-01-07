#include "cPostProcess.h"
#include "cGame.h"
#include "cShader.h"
#include "cGlobals.h"

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


	screenSizeIndex = addUniformVec2("uScreen", game->getScreenDimensions().toVec());
	CHECK_GL_ERROR;
	timeIndex = addUniformFloat("uTime", timer.getTime());
	CHECK_GL_ERROR;
	zoomIndex = addUniformFloat("uZoom", game->getCameraZoom());
	CHECK_GL_ERROR;
}

void cPostProcess::bind()
{
	shader->begin();

	setUniform(screenSizeIndex, game->getScreenDimensions().toVec());
	setUniform(timeIndex, timer.getTime());
	setUniform(zoomIndex, game->getCameraZoom());

	setShaderUniforms();
}
