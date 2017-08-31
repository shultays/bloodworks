#include "cRenderableWithShader.h"
#include "cGame.h"
#include "cShader.h"
#include "cGlobals.h"

void cRenderableWithShader::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	if (game->lastShader != shader)
	{
		game->lastShader = shader;
		shader->begin();
		shader->setViewMatrix(game->getViewMatrix(alignment));
		game->lastAlignment = alignment;
	}
	else if (game->lastAlignment != alignment)
	{
		shader->setViewMatrix(game->getViewMatrix(alignment));
		game->lastAlignment = alignment;
	}

	shader->setCrop(crop);

	setShaderUniforms();
}

void cRenderableWithShader::setShader(const cShaderShr& shader)
{
	this->shader = shader;
}

cRenderableWithShader::cRenderableWithShader(cGame *game, const std::string& shaderPath) : cRenderable(game)
{
	if (shaderPath.size() > 0)
	{
		std::string vs = shaderPath;
		vs.append(".vs");
		std::string ps = shaderPath;
		ps.append(".ps");
		shader = resources.getShader(vs, ps);
	}
}

cRenderableWithShader::cRenderableWithShader(cGame *game, const std::string& vs, const std::string& ps) : cRenderable(game)
{
	shader = resources.getShader(vs, ps);
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

int cUniformDataWithShader::addUniformFloat(const std::string& uniform, float val)
{
	int index = shader->addUniform(uniform, TypeFloat).index;
	setUniform(index, val);
	return index;
}

int cUniformDataWithShader::addUniformVec2(const std::string& uniform, const Vec2& data)
{
	int index = shader->addUniform(uniform, TypeVec2).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformVec3(const std::string& uniform, const Vec3& data)
{
	int index = shader->addUniform(uniform, TypeVec3).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformVec4(const std::string& uniform, const Vec4& data)
{
	int index = shader->addUniform(uniform, TypeVec4).index;
	setUniform(index, data);
	return index;
}

void cUniformDataWithShader::setUniform(int index, float data)
{
	if (index == -1)
	{
		return;
	}
	UniformData uniform;
	uniform.type = TypeFloat;
	uniform.f = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const Vec2& data)
{
	if (index == -1)
	{
		return;
	}
	UniformData uniform;
	uniform.type = TypeVec2;
	uniform.vec2 = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const Vec3& data)
{
	if (index == -1)
	{
		return;
	}
	UniformData uniform;
	uniform.type = TypeVec3;
	uniform.vec3 = data;
	uniforms[index] = uniform;
}

void cUniformDataWithShader::setUniform(int index, const Vec4& data)
{
	if (index == -1)
	{
		return;
	}
	UniformData uniform;
	uniform.type = TypeVec4;
	uniform.vec4 = data;
	uniforms[index] = uniform;
}

int cUniformDataWithShader::addUniformInt(const std::string& uniform, int val)
{
	int index = shader->addUniform(uniform, TypeInt).index;
	setUniform(index, val);
	return index;
}

int cUniformDataWithShader::addUniformIntVec2(const std::string& uniform, const IntVec2& data)
{
	int index = shader->addUniform(uniform, TypeIntVec2).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformIntVec3(const std::string& uniform, const IntVec3& data)
{
	int index = shader->addUniform(uniform, TypeIntVec3).index;
	setUniform(index, data);
	return index;
}

int cUniformDataWithShader::addUniformIntVec4(const std::string& uniform, const IntVec4& data)
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