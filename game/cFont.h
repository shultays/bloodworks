#pragma once

#include "cGlobals.h"
#include "cResources.h"
#include "cRenderable.h"
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

	const char *fontData;

	friend class cTextRenderable;
public:
	cFont(const char *fontData);
	~cFont();

	const char* getFontDataPath() const
	{
		return fontData;
	}

	void deleteSelf()
	{
		delete this;
	}
};

class cTextRenderable : public cRenderableWithShader
{
	cFontShr font;
	float textSize;
	std::string text;
	Vec4 textColor;
	friend class cDebugRenderable;
	virtual void render() override;

public:
	cTextRenderable(cGame *game, cFontShr font, std::string text = "", float textSize = 38.0f, Vec4 textColor = Vec4(1.0f)) : cRenderableWithShader(game, "resources/default.vs", "resources/default.ps")
	{
		this->font = font;
		this->text = text;
		this->textSize = textSize;
		this->textColor = textColor;
	}

	virtual ~cTextRenderable()
	{
		font = nullptr;
	}

	void setText(const std::string& text)
	{
		setText(text.c_str());
	}

	void setText(const char* text)
	{
		this->text = text;
	}

	void setTextSize(float size)
	{
		this->textSize = size;
	}

	void setTextColor(const Vec4& color)
	{
		this->textColor = color;
	}
};