#include "cRenderable.h"
#include "cShader.h"
#include "cGame.h"
#include "cTexture.h"

GLuint quad = -1;

cRenderableGroup::cRenderableGroup(cGame *game, int initialCapacity /*= 2*/) : cRenderable(game)
{
	renderables.reserve(initialCapacity);
}

void cRenderableGroup::render(bool isIdentity, const Mat3& mat)
{
	for (auto& childData : renderables)
	{
		childData.child->render(false, isIdentity ? worldMatrix : mat * worldMatrix);
	}
}

void cRenderableGroup::addRenderable(cRenderable *child)
{
	ChildData childData;
	childData.child = child;
	renderables.push_back(childData);
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

void cRenderableWithShader::render(bool isIdentity, const Mat3& mat)
{
	if (game->lastShader != shader)
	{
		game->lastShader = shader;
		shader->begin();
		shader->setViewMatrix(game->worldViewMatrix);
	}
}

void cTexturedQuadRenderable::render(bool isIdentity, const Mat3& mat)
{
	cRenderableWithShader::render(isIdentity, mat);

	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

	shader->setColor(color);
	shader->setUniform("uTexture", 0);
	shader->setUniform("uTexture2", 1);

	for (int i = 0; i < 4; i++)
	{
		if (texture[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			texture[i]->bindTexture();
		}
	}

	shader->setWorldMatrix(isIdentity ? worldMatrix : worldMatrix * mat);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);


	for (int i = 3; i >= 0; i--)
	{
		if (texture[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glActiveTexture(GL_TEXTURE0);
}

cTexturedQuadRenderable::cTexturedQuadRenderable(cGame *game, const char* texturePath, const char* shaderPath) : cRenderableWithShader(game, shaderPath)
{
	setTexture(texturePath);
	setWorldMatrix(Mat3::translationMatrix(texture[0]->getDimensions().toVec()));
}

