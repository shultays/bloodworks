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


void cRenderableWithShader::render(bool isIdentity, const Mat3& mat)
{

	if (game->lastShader != shader)
	{
		game->lastShader = shader;
		shader->begin();
		shader->setViewMatrix(game->getViewMatrix(alignment));
		game->lastAllignment = alignment;
	}
	else if (game->lastAllignment != alignment)
	{
		shader->setViewMatrix(game->getViewMatrix(alignment));
		game->lastAllignment = alignment;
	}

	for (auto& uniform : uniforms)
	{
		switch (uniform.second.type)
		{
		case TypeFloat:
			shader->setUniform(uniform.first, uniform.second.f);
			break;
		case TypeVec2:
			shader->setUniform(uniform.first, uniform.second.vec2);
			break;
		case TypeVec3:
			shader->setUniform(uniform.first, uniform.second.vec3);
			break;
		case TypeVec4:
			shader->setUniform(uniform.first, uniform.second.vec4);
			break;
		}
	}
}

void cRenderableWithShader::setUniform(int index, float data)
{
	UniformData uniform;
	uniform.type = TypeFloat;
	uniform.f = data;
	uniforms[index] = uniform;
}

void cRenderableWithShader::setUniform(int index, const Vec2& data)
{
	UniformData uniform;
	uniform.type = TypeVec2;
	uniform.vec2 = data;
	uniforms[index] = uniform;
}

void cRenderableWithShader::setUniform(int index, const Vec3& data)
{
	UniformData uniform;
	uniform.type = TypeVec3;
	uniform.vec3 = data;
	uniforms[index] = uniform;
}

void cRenderableWithShader::setUniform(int index, const Vec4& data)
{
	UniformData uniform;
	uniform.type = TypeVec4;
	uniform.vec4 = data;
	uniforms[index] = uniform;
}

void cTexturedQuadRenderable::render(bool isIdentity, const Mat3& mat)
{
	cRenderableWithShader::render(isIdentity, mat);

	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

	shader->setColor(color);
	shader->setTexture0(0);
	shader->setTexture1(1);
	shader->setTexture2(2);
	shader->setTexture3(3);

	for (int i = 3; i >= 0; i--)
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

