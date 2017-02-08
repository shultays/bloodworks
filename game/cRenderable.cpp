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
		if (childData.child->isVisible())
		{
			childData.child->render(false, isIdentity ? worldMatrix : worldMatrix * mat);
		}
	}
}

void cRenderableGroup::addRenderable(cRenderable *child)
{
	ChildData childData;
	childData.child = child;
	child->setAlignment(alignment);
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

	setShaderUniforms();
}

void cUniformDataWithShader::setShaderUniforms()
{
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
		case TypeInt:
			shader->setUniform(uniform.first, uniform.second.i);
			break;
		case TypeIntVec2:
			shader->setUniform(uniform.first, uniform.second.intVec2);
			break;
		case TypeIntVec3:
			shader->setUniform(uniform.first, uniform.second.intVec3);
			break;
		case TypeIntVec4:
			shader->setUniform(uniform.first, uniform.second.intVec4);
			break;
		}
	}
}

int cUniformDataWithShader::addUniformFloat(const std::string uniform, float val)
{
	int index = shader->addUniform(uniform, TypeFloat).index;
	setUniform(index, val);
	return index;
}

int cUniformDataWithShader::addUniformVec2(const std::string uniform, const Vec2& data)
{
	int index = shader->addUniform(uniform, TypeVec2).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformVec3(const std::string uniform, const Vec3& data)
{
	int index = shader->addUniform(uniform, TypeVec3).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformVec4(const std::string uniform, const Vec4& data)
{
	int index = shader->addUniform(uniform, TypeVec4).index;
	setUniform(index, data);
	return index;
}

void cUniformDataWithShader::setUniform(int index, float data)
{
	UniformData uniform;
	uniform.type = TypeFloat;
	uniform.f = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const Vec2& data)
{
	UniformData uniform;
	uniform.type = TypeVec2;
	uniform.vec2 = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const Vec3& data)
{
	UniformData uniform;
	uniform.type = TypeVec3;
	uniform.vec3 = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const Vec4& data)
{
	UniformData uniform;
	uniform.type = TypeVec4;
	uniform.vec4 = data;
	uniforms[index] = uniform;
}

int cUniformDataWithShader::addUniformInt(const std::string uniform, int val)
{
	int index = shader->addUniform(uniform, TypeInt).index;
	setUniform(index, val);
	return index;
}

int cUniformDataWithShader::addUniformIntVec2(const std::string uniform, const IntVec2& data)
{
	int index = shader->addUniform(uniform, TypeIntVec2).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformIntVec3(const std::string uniform, const IntVec3& data)
{
	int index = shader->addUniform(uniform, TypeIntVec3).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformIntVec4(const std::string uniform, const IntVec4& data)
{
	int index = shader->addUniform(uniform, TypeIntVec4).index;
	setUniform(index, data);
	return index;
}

void cUniformDataWithShader::setUniform(int index, int data)
{
	UniformData uniform;
	uniform.type = TypeInt;
	uniform.i = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const IntVec2& data)
{
	UniformData uniform;
	uniform.type = TypeIntVec2;
	uniform.intVec2 = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const IntVec3& data)
{
	UniformData uniform;
	uniform.type = TypeIntVec3;
	uniform.intVec3 = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const IntVec4& data)
{
	UniformData uniform;
	uniform.type = TypeIntVec4;
	uniform.intVec4 = data;
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

cTexturedQuadRenderable::cTexturedQuadRenderable(cGame *game, const std::string& texturePath, const std::string& shaderPath) : cRenderableWithShader(game, shaderPath)
{
	setTexture(texturePath);
	setWorldMatrix(Mat3::translationMatrix(texture[0]->getDimensions().toVec()));
}

