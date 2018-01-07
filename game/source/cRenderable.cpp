#include "cRenderable.h"
#include "cGame.h"

GLuint defaultQuad = -1;
GLuint quadBuffer = -1;

cRenderable::cRenderable(cGame *game)
{
	this->game = game;
	worldMatrix.makeIdentity();
	color = Vec4(1.0f);
	visible = true;
	next = prev = nullptr;
	alignment = RenderableAlignment::world;
}

cRenderable::~cRenderable()
{
	if (game != nullptr)
	{
		game->removeRenderable(this);
	}
}

void cRenderable::setWorldMatrix(const Mat3& worldMatrix)
{
	this->worldMatrix = worldMatrix;
}

void cRenderable::setLevel(int level)
{
	this->level = level;
	if (game)
	{
		game->removeRenderable(this);
		game->addRenderable(this, level);
	}
}

