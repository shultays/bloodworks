#include "cDebugRenderable.h"
#include "cFont.h"
#include "cGlobals.h"


#define MAX_ID 0xFFFFFF
#define MIN_ID 0x0FFFFF

cDebugRenderable::cDebugRenderable()
{
	nextId = MIN_ID;
}

void cDebugRenderable::init()
{
	textRenderable = new cTextRenderable(nullptr, resources.getFont("resources/fontData.txt"));
}

int cDebugRenderable::addTextData(const std::string &string, float x, float y, float time /*= FLT_MAX*/, Vec4 color /*= Vec4(1.0f)*/, float size /*= 24.0f*/)
{
	return addTextData(-1, string, x, y, time, color, size);
}

int cDebugRenderable::addTextData(int id, const std::string &string, float x, float y, float time /*= FLT_MAX*/, Vec4 color /*= Vec4(1.0f)*/, float size /*= 24.0f*/)
{
	TextData data;
	data.text = string;
	data.x = x;
	data.y = y;
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
		textDatas.push_back(data);
	}
	else
	{
		data.id = id;
		for (int i = 0; i < textDatas.size(); i++)
		{
			if (textDatas[i].id == id)
			{
				textDatas[i] = data;
				return id;
			}
		}
		textDatas.push_back(data);
	}
	return id;
}

void cDebugRenderable::render()
{
	if (game == nullptr)
	{
		return;
	}
	textRenderable->game = game;
	Vec2 halfScreen = game->getScreenDimensions().toVec() * 0.5f;
	for (auto& textData : textDatas)
	{
		textRenderable->setText(textData.text);
		textRenderable->setTextColor(textData.color);
		textRenderable->setTextSize(textData.size);
		textRenderable->setPosition(Vec2(textData.x - halfScreen.w, - textData.y + halfScreen.h - textData.size));
		textRenderable->render();
	}
}

void cDebugRenderable::tick(float dt)
{
	for (int i = 0; i < textDatas.size(); i++)
	{
		if (textDatas[i].time < FLT_MAX)
		{
			textDatas[i].time -= dt;
		}

		if (textDatas[i].time <= 0.0f)
		{
			textDatas[i] = textDatas[textDatas.size() - 1];
			textDatas.resize(textDatas.size() - 1);
			i--;
		}
	}
}


