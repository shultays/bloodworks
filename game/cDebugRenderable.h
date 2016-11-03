#pragma once

class cTextRenderable;

#include "cMat.h"
#include "cVec.h"
#include <GL/glew.h>
#include <vector>

class cDebugRenderable
{
	cTextRenderable *textRenderable;

	struct TextData  
	{
		int id;
		std::string text;
		Vec4 color;
		float size;
		float time;
		Vec2 pos;
	};

	std::vector<TextData> textData;
	int nextId;

	struct LineData
	{
		int id;
		float time;
		Vec2 pos0;
		Vec2 pos1;
		Vec4 color;
	};

	struct LinetGPUData
	{
		Vec2 pos0;
		Vec4 color0;
		Vec2 pos1;
		Vec4 color1;
	};

	LinetGPUData *lineGPUData;

	std::vector<LineData> lineData;
	GLuint lineGPUBuffer;
public:

	const float infinite = FLT_MAX;
	cDebugRenderable();

	void init();

	int addTextData(int id, const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);
	int addTextData(const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);

	int addLineData(int id, const Vec2& pos0, const Vec2& pos1, float time = 0.0f, Vec4 color = Vec4(1.0f));
	int addLineData(const Vec2& pos0, const Vec2& pos1, float time = 0.0f, Vec4 color = Vec4(1.0f));

	void render();
	void tick(float dt);
};
