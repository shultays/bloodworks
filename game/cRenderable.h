#pragma once

#include "cGlobals.h"
#include "cGame.h"
#include "cShader.h"
#include "cTexture.h"
#include <string>
#include <GL/glew.h>


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

public:
	cRenderable(cGame *game)
	{
		this->game = game;
		worldMatrix.makeIdentity();
	}

	virtual ~cRenderable() {}

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
};

class cRenderableWithShader : public cRenderable
{
protected:
	cShaderShr shader;
	virtual void render() override
	{
		if (game->lastShader != shader)
		{
			game->lastShader = shader;
			shader->begin();
			shader->setViewMatrix(game->worldViewMatrix);
		}
	}
public:
	cRenderableWithShader(cGame *game, const char* shaderPath) : cRenderable(game)
	{
		std::string vs = shaderPath;
		vs.append(".vs");
		std::string ps = shaderPath;
		ps.append(".ps");
		shader = resources.getShader(vs.c_str(), ps.c_str());
	}

	cRenderableWithShader(cShaderShr shader) : cRenderable(game)
	{
		this->shader = shader;
	}

	cRenderableWithShader(cGame *game, const char* vs, const char* ps) : cRenderable(game)
	{
		shader = resources.getShader(vs, ps);
	}

	virtual ~cRenderableWithShader() 
	{
		shader = nullptr;
	}
};

class cTexturedQuadRenderable : public cRenderableWithShader
{
protected:
	cTextureShr texture;

	virtual void render() override
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
public:
	cTexturedQuadRenderable(cGame *game, const char* texturePath, const char* shaderPath) : cRenderableWithShader(game, shaderPath)
	{
		texture = resources.getTexture(texturePath);
	}
};