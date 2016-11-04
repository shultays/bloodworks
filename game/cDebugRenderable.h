#pragma once

class cTextRenderable;

#include "cMat.h"
#include "cVec.h"
#include "cResources.h"
#include <GL/glew.h>
#include <vector>

class cDebugRenderable
{
	cTextRenderable *textRenderable;

	cShaderShr lineShader;
	cTextureShr whiteTexture;

	struct TextData  
	{
		bool isScreen;
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

	int addTextInternal(int id, const std::string &string, bool isScreen, float x, float y, float time, Vec4 color, float size);
public:

	const float infinite = FLT_MAX;
	cDebugRenderable();
	~cDebugRenderable();
	void freeAll();

	void init();

	int addText(int id, const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);
	int addText(const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);
	int addScreenText(int id, const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);
	int addScreenText(const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);

	int addLine(int id, const Vec2& pos0, const Vec2& pos1, float time = 0.0f, Vec4 color = Vec4(1.0f));
	int addLine(const Vec2& pos0, const Vec2& pos1, float time = 0.0f, Vec4 color = Vec4(1.0f));

	void render();
	void tick(float dt);
};
