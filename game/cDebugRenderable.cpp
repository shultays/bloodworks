#include "cDebugRenderable.h"
#include "cTextRenderable.h"
#include "cGlobals.h"
#include "cShader.h"
#include "cTexture.h"
#include "cGame.h"
#include <assert.h>


#define MAX_ID 0xFFFFFF
#define MIN_ID 0x0FFFFF

#define MAX_LINE 2048


int cDebugRenderable::addTextInternal(int id, const std::string &string, float x, float y, float time, Vec4 color, float size, TextAlignment textAlignment, RenderableAlignment alignment)
{
	TextData data;
	data.text = string;
	data.pos = Vec2(x, y);
	data.time = time;
	data.color = color;
	data.size = size;
	data.alignment = alignment;
	data.textAlignment = textAlignment;
	if (id == -1)
	{
		id = data.id = nextId++;
		if (nextId > MAX_ID)
		{
			nextId = MIN_ID;
		}
		textData.push_back(data);
	}
	else
	{
		data.id = id;
		for (int i = 0; i < textData.size(); i++)
		{
			if (textData[i].id == id)
			{
				textData[i] = data;
				return id;
			}
		}
		textData.push_back(data);
	}
	return id;
}

cDebugRenderable::cDebugRenderable()
{
	nextId = MIN_ID;
	lineGPUData = new LinetGPUData[MAX_LINE];
	lineGPUBuffer = -1;
}

cDebugRenderable::~cDebugRenderable()
{
	freeAll();
}

void cDebugRenderable::freeAll()
{
	SAFE_DELETE(textRenderable);
	SAFE_DELETE(lineGPUData);
	if (lineGPUBuffer != -1)
	{
		glDeleteBuffers(1, &lineGPUBuffer);
	}
	lineShader = nullptr;
	textRenderable = nullptr;
	whiteTexture = nullptr;
}

void cDebugRenderable::init()
{
	textRenderable = new cTextRenderable(nullptr, resources.getFont("resources/fontData.txt"));
	lineShader = resources.getShader("resources/default.vs", "resources/default.ps");
	whiteTexture = resources.getTexture("resources/white.png", true);

	glGenBuffers(1, &lineGPUBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineGPUBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LinetGPUData) * MAX_LINE, NULL, GL_DYNAMIC_DRAW);
}

int cDebugRenderable::addText(const std::string &string, float x, float y, float time /*= FLT_MAX*/, Vec4 color /*= Vec4(1.0f)*/, float size /*= 24.0f*/, TextAlignment textAlignment, RenderableAlignment alignment)
{
	return addTextInternal(-1, string, x, y, time, color, size, textAlignment, alignment);
}

int cDebugRenderable::addText(int id, const std::string &string, float x, float y, float time /*= FLT_MAX*/, Vec4 color /*= Vec4(1.0f)*/, float size /*= 24.0f*/, TextAlignment textAlignment, RenderableAlignment alignment)
{
	return addTextInternal(id, string, x, y, time, color, size, textAlignment, alignment);
}

int cDebugRenderable::addLine(int id, const Vec2& pos0, const Vec2& pos1, float time /*= 0.0f*/, Vec4 color /*= Vec4(1.0f)*/)
{
	LineData data;
	data.pos0 = pos0;
	data.pos1 = pos1;
	data.time = time;
	data.color = color;

	if (id == -1)
	{
		id = data.id = nextId++;
		if (nextId > MAX_ID)
		{
			nextId = MIN_ID;
		}
		if (lineData.size() < MAX_LINE)
		{
			lineData.push_back(data);
		}
	}
	else
	{
		data.id = id;
		for (int i = 0; i < lineData.size(); i++)
		{
			if (lineData[i].id == id)
			{
				lineData[i] = data;
				return id;
			}
		}
		if (lineData.size() < MAX_LINE)
		{
			lineData.push_back(data);
		}
	}
	return id;
}

int cDebugRenderable::addLine(const Vec2& pos0, const Vec2& pos1, float time /*= 0.0f*/, Vec4 color /*= Vec4(1.0f)*/)
{
	return addLine(-1, pos0, pos1, time, color);
}

void cDebugRenderable::addCircle(const Vec2& center, float radius, float time /*= 0.0f*/, Vec4 color /*= Vec4(1.0f)*/)
{
	int lineCount = 7 + (int)round(radius / 8);

	Vec2 oldVec = Vec2::fromAngle(-1 * pi_2 / lineCount) * radius;
	for (int i = 0; i < lineCount; i++)
	{
		Vec2 newVec = Vec2::fromAngle(i * pi_2 / lineCount) * radius;
		addLine(center + newVec, center + oldVec, time, color);
		oldVec = newVec;
	}
}

void cDebugRenderable::render()
{
	if (game == nullptr)
	{
		return;
	}
	textRenderable->game = game;
	Vec2 halfScreen = game->getScreenDimensions().toVec() * 0.5f;
	for (auto& textData : textData)
	{
		textRenderable->setText(textData.text);
		textRenderable->setTextColor(textData.color);
		textRenderable->setTextSize(textData.size);
		textRenderable->setTextAlignment(textData.textAlignment);
		textRenderable->setWorldMatrix(Mat3::translationMatrix(Vec2(textData.pos.x, textData.pos.y)));
		textRenderable->setAlignment((RenderableAlignment)textData.alignment);
		textRenderable->render(true, Mat3::identity(), Rect::invalid());
	}

	if (lineData.size() > 0)
	{
		for (int i = 0; i < lineData.size(); i++)
		{
			lineGPUData[i].pos0 = lineData[i].pos0;
			lineGPUData[i].color0 = lineData[i].color;
			lineGPUData[i].pos1 = lineData[i].pos1;
			lineGPUData[i].color1 = lineData[i].color;
		}
		glBindBuffer(GL_ARRAY_BUFFER, lineGPUBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LinetGPUData) * lineData.size(), lineGPUData);
		
		lineShader->bindPosition(sizeof(float) * 6, 0);
		lineShader->setAttributeUV(Vec2::zero());
		lineShader->bindColor(sizeof(float) * 6, sizeof(float) * 2);

		lineShader->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		whiteTexture->bindTexture();
		lineShader->setWorldMatrix(Mat3::identity());
		lineShader->setViewMatrix(game->getViewMatrix(RenderableAlignment::world));
		glDrawArrays(GL_LINES, 0, (int)lineData.size() * 2);

	}
	textRenderable->game = nullptr;
}

void cDebugRenderable::tick(float dt)
{
	for (int i = 0; i < textData.size(); i++)
	{
		if (textData[i].time < FLT_MAX)
		{
			textData[i].time -= dt;
		}

		if (textData[i].time <= 0.0f)
		{
			textData[i] = textData[textData.size() - 1];
			textData.resize(textData.size() - 1);
			i--;
		}
	}
	for (int i = 0; i < lineData.size(); i++)
	{
		if (lineData[i].time < FLT_MAX)
		{
			lineData[i].time -= dt;
		}

		if (lineData[i].time <= 0.0f)
		{
			lineData[i] = lineData[lineData.size() - 1];
			lineData.resize(lineData.size() - 1);
			i--;
		}
	}
}

void cDebugRenderable::removeText(int id)
{
	for (int i = 0; i < textData.size(); i++)
	{
		if (textData[i].id == id)
		{
			textData[i] = textData[textData.size() - 1];
			textData.resize(textData.size() - 1);
			return;
		}
	}
}

void cDebugRenderable::removeLine(int id)
{
	for (int i = 0; i < lineData.size(); i++)
	{
		if (lineData[i].id == id)
		{
			lineData[i] = lineData[lineData.size() - 1];
			lineData.resize(lineData.size() - 1);
			return;
		}
	}
}

