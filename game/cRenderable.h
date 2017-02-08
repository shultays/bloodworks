#pragma once

#include "cGlobals.h"
#include <string>
#include <GL/glew.h>

class cGame;
enum class RenderableAlignment;
extern GLuint quad;

class cRenderable
{
	friend class cGame;
	friend class cRenderableGroup;
	friend class cDebugRenderable;
protected:
	cGame *game;
	virtual void render()
	{
		render(true, Mat3::identity());
	}
	Mat3 worldMatrix;
	Vec4 color;
	bool visible;
	RenderableAlignment alignment;

	int level;

	cRenderable *next;
	cRenderable *prev;
public:
	cRenderable(cGame *game);

	virtual void render(bool isIdentity, const Mat3& mat) = 0;

	bool isVisible() const
	{
		return visible;
	}

	RenderableAlignment getAlignment() const
	{
		return alignment;
	}

	void setVisible(bool visible)
	{
		this->visible = visible;
	}

	virtual void setAlignment(RenderableAlignment alignment)
	{
		this->alignment = alignment;
	}

	virtual ~cRenderable();

	const Mat3& getWorldMatrix() const
	{
		return worldMatrix;
	}
	
	const Vec2& getPosition() const
	{
		return worldMatrix.row2.vec2;
	}
	void setWorldMatrix(const Mat3& worldMatrix);

	virtual void setColor(const Vec4& color)
	{
		this->color = color;
	}

	const Vec4& getColor() const
	{
		return color;
	}

	void setLevel(int level);
};

class cRenderableGroup : public cRenderable
{
	struct ChildData
	{
		cRenderable *child;
	};
	std::vector<ChildData> renderables;

public:
	cRenderableGroup(cGame *game, int initialCapacity = 2);
	~cRenderableGroup()
	{
		for (auto& childData : renderables)
		{
			SAFE_DELETE(childData.child);
		}
	}

	virtual void render(bool isIdentity, const Mat3& mat) override;
	void addRenderable(cRenderable *child);
	virtual void setAlignment(RenderableAlignment alignment) override
	{
		cRenderable::setAlignment(alignment);
		for (auto& childData : renderables)
		{
			childData.child->setAlignment(alignment);
		}
	}

	virtual void setColor(const Vec4& color) override
	{
		cRenderable::setColor(color);

		for (auto& childData : renderables)
		{
			childData.child->setColor(color);
		}
	}
};

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
	void setShader(cShaderShr shader)
	{
		this->shader = shader;
	}

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
protected:
	virtual void render(bool isIdentity, const Mat3& mat) override;

public:
	cRenderableWithShader(cGame *game, const std::string& shaderPath) : cRenderable(game)
	{
		std::string vs = shaderPath;
		vs.append(".vs");
		std::string ps = shaderPath;
		ps.append(".ps");
		shader = resources.getShader(vs, ps);
	}

	cRenderableWithShader(cGame *game, cShaderShr shader) : cRenderable(game)
	{
		this->shader = shader;
	}

	virtual ~cRenderableWithShader()
	{
	}

	cRenderableWithShader(cShaderShr shader) : cRenderable(game)
	{
		this->shader = shader;
	}

	cRenderableWithShader(cGame *game, const std::string& vs, const std::string& ps) : cRenderable(game)
	{
		shader = resources.getShader(vs, ps);
	}

};

class cTexturedQuadRenderable : public cRenderableWithShader
{
protected:
	cTextureShr texture[4];

public:
	cTexturedQuadRenderable(cGame *game, const std::string& texturePath, const std::string& shaderPath);

	virtual ~cTexturedQuadRenderable()
	{
		for (int i = 0; i < 4; i++)
		{
			texture[i] = nullptr;
		}
	}

	virtual void render(bool isIdentity, const Mat3& mat) override;

	void setTexture(const std::string& texturePath)
	{
		setTexture(0, texturePath);
	}

	void setTexture(int i, const std::string& texturePath)
	{
		texture[i] = resources.getTexture(texturePath);
	}

	cTextureShr& getTexture()
	{
		return getTexture(0);
	}

	cTextureShr& cTexturedQuadRenderable::getTexture(int i)
	{
		return texture[i];
	}
};