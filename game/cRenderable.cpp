#include "cRenderable.h"
#include "cShader.h"
#include "cGame.h"
#include "cTexture.h"

GLuint quad = -1;

cRenderableGroup::cRenderableGroup(cGame *game, int initialCapacity /*= 2*/) : cRenderable(game)
{
	renderables.reserve(initialCapacity);
}

void cRenderableGroup::render()
{
	if (dirty)
	{
		dirty = false;

		for (auto& childData : renderables)
		{
			childData.child->setWorldMatrix(childData.localMatrix * worldMatrix);
		}
	}

	for (auto& childData : renderables)
	{
		childData.child->render();
	}
}

void cRenderableGroup::addRenderable(cRenderable *child, Mat3 localMatrix)
{
	ChildData childData;
	childData.child = child;
	childData.localMatrix = localMatrix;
	renderables.push_back(childData);
}

cRenderable::~cRenderable()
{
	if (game != game)
	{
		game->removeRenderable(this);
	}
}

void cRenderable::setWorldMatrix(const Mat3& worldMatrix)
{
	this->worldMatrix = worldMatrix;
}

void cRenderableWithShader::render()
{
	if (game->lastShader != shader)
	{
		game->lastShader = shader;
		shader->begin();
		shader->setViewMatrix(game->worldViewMatrix);
	}
}

void cTexturedQuadRenderable::render()
{
	cRenderableWithShader::render();

	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

	shader->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	texture->bindTexture();
	shader->setWorldMatrix(worldMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);
}
