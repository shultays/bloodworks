#pragma once

#include <vector>
#include "cRenderable.h"
#include "cShader.h"
#include "cTexture.h"
#include "cGame.h"

#define MAX_QUAD 10000

class cParticle : public cRenderable
{
	struct VertexData
	{
		Vec2 pos;
		Vec2 initialScale;
		Vec2 uv;
		Vec4 color;
		Vec2 moveSpeed;
		Vec2 scaleSpeed;
		float time;
		float rotate;
		float rotateSpeed;
		float fadeOutSpeed;
	};

	struct QuadData
	{
		VertexData vertices[4];
	};

	
	std::vector<QuadData> quads;

	GLuint quadBuffer;

	float time;
	cShaderShr shader;
	cTextureShr texture;
public:

	cParticle(cGame* game) : cRenderable(game)
	{
		glGenBuffers(1, &quadBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadData) * MAX_QUAD, NULL, GL_DYNAMIC_DRAW);
		time = timer.getTime();

		shader = resources.getShader("resources/particleShader.vs", "resources/particleShader.ps");

		shader->addAttribute("pos", TypeVec2);
		shader->addAttribute("initialScale", TypeVec2);
		shader->addAttribute("uv", TypeVec2);
		shader->addAttribute("color", TypeVec4);
		shader->addAttribute("moveSpeed", TypeVec2);
		shader->addAttribute("scaleSpeed", TypeVec2);
		shader->addAttribute("time", TypeFloat);
		shader->addAttribute("rotate", TypeFloat);
		shader->addAttribute("rotateSpeed", TypeFloat);
		shader->addAttribute("fadeOutSpeed", TypeFloat);

		shader->addUniform("currentTime", TypeFloat);

		texture = resources.getTexture("resources/white.png");
	}

	~cParticle()
	{
		shader = nullptr;
		texture = nullptr;
		glDeleteBuffers(1, &quadBuffer);
	}

	void addParticle(const Vec2& pos)
	{
		QuadData quadData;

		for (int i = 0; i < 4; i++)
		{
			quadData.vertices[i].pos = pos;
			quadData.vertices[i].initialScale = Vec2(10.0f);
			quadData.vertices[i].color = Vec4(1.0);
			quadData.vertices[i].moveSpeed = Vec2(0.0f);
			quadData.vertices[i].scaleSpeed = Vec2(0.0f);
			quadData.vertices[i].time = timer.getTime() - time;
			quadData.vertices[i].rotate = 0.0f;
			quadData.vertices[i].rotateSpeed = 0.0f;
			quadData.vertices[i].fadeOutSpeed = 0.0f;
		}
		quadData.vertices[0].uv = Vec2(0.0f, 0.0f);
		quadData.vertices[1].uv = Vec2(0.0f, 1.0f);
		quadData.vertices[2].uv = Vec2(1.0f, 1.0f);
		quadData.vertices[3].uv = Vec2(1.0f, 0.0f);


		glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(QuadData) * quads.size(), sizeof(QuadData), &quadData);

		quads.push_back(quadData);

	}

	virtual void render(bool isIdentity, const Mat3& mat)
	{
		if (quads.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);

			game->lastShader = nullptr;

			shader->begin();

			shader->bindAttribute("pos", sizeof(VertexData), offsetof(VertexData, pos));
			shader->bindAttribute("initialScale", sizeof(VertexData), offsetof(VertexData, initialScale));
			shader->bindAttribute("uv", sizeof(VertexData), offsetof(VertexData, uv));
			shader->bindAttribute("color", sizeof(VertexData), offsetof(VertexData, color));
			shader->bindAttribute("moveSpeed", sizeof(VertexData), offsetof(VertexData, moveSpeed));
			shader->bindAttribute("scaleSpeed", sizeof(VertexData), offsetof(VertexData, scaleSpeed));
			shader->bindAttribute("time", sizeof(VertexData), offsetof(VertexData, time));
			shader->bindAttribute("rotate", sizeof(VertexData), offsetof(VertexData, rotate));
			shader->bindAttribute("rotateSpeed", sizeof(VertexData), offsetof(VertexData, rotateSpeed));
			shader->bindAttribute("fadeOutSpeed", sizeof(VertexData), offsetof(VertexData, fadeOutSpeed));

			shader->setUniform("currentTime", 0.0f);

			glActiveTexture(GL_TEXTURE0);
			texture->bindTexture();
			shader->setUniform("uTexture", 0);

			shader->setViewMatrix(game->getViewMatrix());

			glEnableVertexAttribArray(0);
			glDrawArrays(GL_QUADS, 0, (int)quads.size() * 4);

			glDisableVertexAttribArray(0);

			glDisable(GL_TEXTURE_2D);
		}

	}
};