#pragma once

class cTextRenderable;

#include "cMat.h"
#include "cVec.h"
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
		float x, y;
		float time;
	};

	std::vector<TextData> textDatas;
	int nextId;

public:

	const float infinite = FLT_MAX;
	cDebugRenderable();

	void init();

	int addTextData(int id, const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);
	int addTextData(const std::string &string, float x, float y, float time = 0.0f, Vec4 color = Vec4(1.0f), float size = 24.0f);

	void render();
	void tick(float dt);
};
