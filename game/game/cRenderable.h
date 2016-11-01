#pragma once

#include "cGlobals.h"
#include <string>

class cRenderable
{
	friend class cGame;
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
	static GLuint quad;

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
		if (quad == -1)
		{
			GLfloat vertexData[] =
			{
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
				1.0f,  -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				-1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			};


			//IBO data
			GLuint indexData[] = { 0, 1, 2, 3 };

			//Create VBO
			glGenBuffers(1, &quad);
			glBindBuffer(GL_ARRAY_BUFFER, quad);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

			glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		texture = resources.getTexture(texturePath);
	}
	void setWorldFrame(const Mat3& mat);
};