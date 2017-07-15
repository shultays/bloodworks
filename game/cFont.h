#pragma once

#include "cGlobals.h"
#include <fstream>      // std::ifstream

class cFont
{
	struct CharInfo
	{
		int x, y;
		int w, h;
		GLuint vbo;
	};

	CharInfo charInfos[128];
	int leftPadding, rightPadding, topPadding, bottomPadding;
	cTextureShr texture;

	int maxWidth;
	int maxHeight;

	std::string fontData;

	friend class cTextRenderable;
	float defaultSize;
public:
	cFont(const std::string& fontData);
	~cFont();

	const std::string& getFontDataPath() const
	{
		return fontData;
	}

	void deleteSelf()
	{
		delete this;
	}
};

enum class TextAlignment
{
	left,
	center,
	right
};

enum class VerticalTextAlignment
{
	top,
	mid,
	bottom
};
