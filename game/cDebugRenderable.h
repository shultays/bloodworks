#pragma once

class cTextRenderable;

#include "cMat.h"
#include "cVec.h"
#include "cResources.h"
#include <GL/glew.h>
#include <vector>

enum class RenderableAlignment;
enum class TextAlignment;

class cDebugRenderable
{
	cTextRenderable *textRenderable;

	cShaderShr lineShader;
	cTextureShr whiteTexture;

	struct TextData  
	{
		int id;
		std::string text;
		Vec4 color;
		float size;
		float time;
		Vec2 pos;
		TextAlignment textAlignment;
		RenderableAlignment alignment;
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

	int addTextInternal(int id, const std::string &string, float x, float y, float time, Vec4 color, float size, TextAlignment textAlignment, RenderableAlignment alignment);
public:

	const float infinite = FLT_MAX;
	cDebugRenderable();
	~cDebugRenderable();
	void freeAll();

	void init();

	void removeText(int id);
	void removeLine(int id);

	int addText(int id, const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f, TextAlignment textAlignment = (TextAlignment)0, RenderableAlignment alignment = (RenderableAlignment)0);
	int addText(const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f, TextAlignment textAlignment = (TextAlignment)0, RenderableAlignment alignment = (RenderableAlignment)0);

	int addLine(int id, const Vec2& pos0, const Vec2& pos1, float time = 0.0f, Vec4 color = Vec4(1.0f));
	int addLine(const Vec2& pos0, const Vec2& pos1, float time = 0.0f, Vec4 color = Vec4(1.0f));

	void addCircle(const Vec2& center, float radius, float time = 0.0f, Vec4 color = Vec4(1.0f));

	void render();
	void tick(float dt);
};
