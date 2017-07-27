#pragma once

#include "cRenderable.h"

class cUniformDataWithShader
{
protected:
	cShaderShr shader;

	struct UniformData
	{
		int type;
		union
		{
			Vec4 vec4;
			Vec3 vec3;
			Vec2 vec2;
			float f;

			IntVec4 intVec4;
			IntVec3 intVec3;
			IntVec2 intVec2;
			int i;
		};

		UniformData() {}
		UniformData& operator=(const UniformData& other)
		{
			this->type = other.type;
			this->vec4 = other.vec4;
			return *this;
		}
	};

	std::unordered_map<int, UniformData> uniforms;
public:

	cShaderShr getShader() const
	{
		return shader;
	}

	cUniformDataWithShader()
	{
		shader = nullptr;
	}

	virtual ~cUniformDataWithShader()
	{
		shader = nullptr;
	}

	void setShaderUniforms();

	int addUniformFloat(const std::string uniform, float val);
	int addUniformVec2(const std::string uniform, const Vec2& data);
	int addUniformVec3(const std::string uniform, const Vec3& data);
	int addUniformVec4(const std::string uniform, const Vec4& data);
	void setUniform(int index, float data);
	void setUniform(int index, const Vec2& data);
	void setUniform(int index, const Vec3& data);
	void setUniform(int index, const Vec4& data);

	int addUniformInt(const std::string uniform, int val);
	int addUniformIntVec2(const std::string uniform, const IntVec2& data);
	int addUniformIntVec3(const std::string uniform, const IntVec3& data);
	int addUniformIntVec4(const std::string uniform, const IntVec4& data);
	void setUniform(int index, int data);
	void setUniform(int index, const IntVec2& data);
	void setUniform(int index, const IntVec3& data);
	void setUniform(int index, const IntVec4& data);
};

class cRenderableWithShader : public cRenderable, public cUniformDataWithShader
{
public:
	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;

	cRenderableWithShader(cGame *game, const std::string& shaderPath);

	cRenderableWithShader(cGame *game, cShaderShr shader) : cRenderable(game)
	{
		this->shader = shader;
	}

	virtual ~cRenderableWithShader()
	{
	}

	virtual void setShader(const cShaderShr& shader) override;

	cRenderableWithShader(cShaderShr shader) : cRenderable(game)
	{
		this->shader = shader;
	}

	cRenderableWithShader(cGame *game, const std::string& vs, const std::string& ps);
};
