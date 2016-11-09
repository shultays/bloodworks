#pragma once

#include "cGlobals.h"
#include <string>
#include <GL/glew.h>

class cGame;
extern GLuint quad;

class cRenderable
{
	friend class cGame;
	friend class cRenderableGroup;
	friend class cDebugRenderable;
protected:
	cGame *game;
	virtual void render() = 0;
	Mat3 worldMatrix;
	Vec4 color;

public:
	cRenderable(cGame *game)
	{
		this->game = game;
		worldMatrix.makeIdentity();
		color = Vec4(1.0f);
	}

	virtual ~cRenderable();

	void setPosition(float x, float y)
	{
		setPosition(Vec2(x, y));
	}

	void setPosition(Vec2 pos)
	{
		worldMatrix.row2.vec2 = pos;
	}

	void setSize(float x, float y)
	{
		setSize(Vec2(x, y));
	}

	void setSize(Vec2 pos)
	{
		worldMatrix.row0.x = pos.x;
		worldMatrix.row1.y = pos.y;
	}

	const Vec2& getPosition() const
	{
		return worldMatrix.row2.vec2;
	}

	Vec2 getSize() const
	{
		return Vec2(worldMatrix.row0.x, worldMatrix.row1.y);
	}

	const Mat3& getWorldMatrix() const
	{
		return worldMatrix;
	}

	void setWorldMatrix(const Mat3& worldMatrix);

	void setColor(const Vec4& color)
	{
		this->color = color;
	}
};

class cRenderableGroup : public cRenderable
{
	struct ChildData
	{
		Mat3 localMatrix;
		cRenderable *child;
	};
	std::vector<ChildData> renderables;
	bool dirty;

protected:
	virtual void render() override;

public:
	void addRenderable(cRenderable *child, Mat3 localMatrix);
	cRenderableGroup(cGame *game, int initialCapacity = 2);
	~cRenderableGroup()
	{
		for (auto& childData : renderables)
		{
			SAFE_DELETE(childData.child);
		}
	}

	void setPosition(float x, float y)
	{
		cRenderable::setPosition(x, y);
		dirty = true;
	}

	void setPosition(Vec2 pos)
	{
		cRenderable::setPosition(pos);
		dirty = true;
	}

	void setSize(float x, float y)
	{
		cRenderable::setSize(x, y);
		dirty = true;
	}

	void setSize(Vec2 pos)
	{
		cRenderable::setSize(pos);
		dirty = true;
	}

	void setWorldMatrix(const Mat3& worldMatrix)
	{
		cRenderable::setWorldMatrix(worldMatrix);
		dirty = true;
	}

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
	virtual void render() override;
public:
	cRenderableWithShader(cGame *game, const char* shaderPath) : cRenderable(game)
	{
		std::string vs = shaderPath;
		vs.append(".vs");
		std::string ps = shaderPath;
		ps.append(".ps");
		shader = resources.getShader(vs.c_str(), ps.c_str());
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
};

class cTexturedQuadRenderable : public cRenderableWithShader
{
protected:
	cTextureShr texture[4];

	virtual void render() override;
public:
	cTexturedQuadRenderable(cGame *game, const char* texturePath, const char* shaderPath);

	virtual ~cTexturedQuadRenderable()
	{
		for (int i = 0; i < 4; i++)
		{
			texture[i] = nullptr;
		}
	}

	void setTexture(const char* texturePath)
	{
		setTexture(0, texturePath);
	}

	void setTexture(int i, const char* texturePath)
	{
		texture[i] = resources.getTexture(texturePath);
	}

};