#include "cDebugRenderable.h"
#include "cFont.h"
#include "cGlobals.h"


#define MAX_ID 0xFFFFFF
#define MIN_ID 0x0FFFFF

#define MAX_LINE 1000

cShaderShr lineShader; // todo move
cTextureShr whiteTexture; // todo move

cDebugRenderable::cDebugRenderable()
{
	nextId = MIN_ID;
	lineGPUData = new LinetGPUData[MAX_LINE];
}

void cDebugRenderable::init()
{
	textRenderable = new cTextRenderable(nullptr, resources.getFont("resources/fontData.txt"));

	lineShader = resources.getShader("resources/default.vs", "resources/default.ps");
	whiteTexture = resources.getTexture("resources/white.png");

	glGenBuffers(1, &lineGPUBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineGPUBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LinetGPUData) * MAX_LINE, nullptr, GL_DYNAMIC_DRAW);

}

int cDebugRenderable::addTextData(const std::string &string, float x, float y, float time /*= FLT_MAX*/, Vec4 color /*= Vec4(1.0f)*/, float size /*= 24.0f*/)
{
	return addTextData(-1, string, x, y, time, color, size);
}

int cDebugRenderable::addTextData(int id, const std::string &string, float x, float y, float time /*= FLT_MAX*/, Vec4 color /*= Vec4(1.0f)*/, float size /*= 24.0f*/)
{
	TextData data;
	data.text = string;
	data.pos = Vec2(x, y);
	data.time = time;
	data.color = color;
	data.size = size;
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

int cDebugRenderable::addLineData(int id, const Vec2& pos0, const Vec2& pos1, float time /*= 0.0f*/, Vec4 color /*= Vec4(1.0f)*/)
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
		lineData.push_back(data);
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
		lineData.push_back(data);
	}
	return id;
}

int cDebugRenderable::addLineData(const Vec2& pos0, const Vec2& pos1, float time /*= 0.0f*/, Vec4 color /*= Vec4(1.0f)*/)
{
	return addLineData(-1, pos0, pos1, time, color);
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
		textRenderable->setPosition(Vec2(textData.pos.x - halfScreen.w, - textData.pos.y + halfScreen.h - textData.size));
		textRenderable->render();
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
		glDrawArrays(GL_LINES, 0, (int)lineData.size() * 2);

	}
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


