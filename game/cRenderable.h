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

	void setAlignment(RenderableAlignment alignment)
	{
		this->alignment = alignment;
	}

	virtual ~cRenderable();

	const Mat3& getWorldMatrix() const
	{
		return worldMatrix;
	}

	void setWorldMatrix(const Mat3& worldMatrix);

	void setColor(const Vec4& color)
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

	void setColor(const Vec4& color)
	{
		cRenderable::setColor(color);

		for (auto& childData : renderables)
		{
			childData.child->setColor(color);
		}
	}
};

class cRenderableWithShader : public cRenderable
{
protected:
	cShaderShr shader;
	virtual void render(bool isIdentity, const Mat3& mat) override;

	struct UniformData
	{
		int type;
		union
		{
			Vec4 vec4;
			Vec3 vec3;
			Vec2 vec2;
			float f;
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
	cRenderableWithShader(cGame *game, const char* shaderPath) : cRenderable(game)
	{
		std::string vs = shaderPath;
		vs.append(".vs");
		std::string ps = shaderPath;
		ps.append(".ps");
		shader = resources.getShader(vs.c_str(), ps.c_str());
	}

	cRenderableWithShader(cGame *game, cShaderShr shader) : cRenderable(game)
	{
		this->shader = shader;
	}

	virtual ~cRenderableWithShader()
	{
		shader = nullptr;
	}

	cRenderableWithShader(cShaderShr shader) : cRenderable(game)
	{
		this->shader = shader;
	}

	cRenderableWithShader(cGame *game, const char* vs, const char* ps) : cRenderable(game)
	{
		shader = resources.getShader(vs, ps);
	}

	void setShader(cShaderShr shader)
	{
		this->shader = shader;
	}

	void setUniform(int index, float data);
	void setUniform(int index, const Vec2& data);
	void setUniform(int index, const Vec3& data);
	void setUniform(int index, const Vec4& data);
};

class cTexturedQuadRenderable : public cRenderableWithShader
{
protected:
	cTextureShr texture[4];

public:
	cTexturedQuadRenderable(cGame *game, const char* texturePath, const char* shaderPath);

	virtual ~cTexturedQuadRenderable()
	{
		for (int i = 0; i < 4; i++)
		{
			texture[i] = nullptr;
		}
	}

	virtual void render(bool isIdentity, const Mat3& mat) override;

	void setTexture(const char* texturePath)
	{
		setTexture(0, texturePath);
	}

	void setTexture(int i, const char* texturePath)
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