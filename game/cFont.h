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
	float defaultSize;
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

enum class TextAlignment
{
	left,
	center,
	right
};

class cTextRenderable : public cRenderableWithShader
{
private:
	cFontShr font;
	float textSize;
	std::string text;
	Vec4 textColor;
	friend class cDebugRenderable;
	float length;
	bool lengthDirty;
	TextAlignment textAlignment;

	virtual void render(bool isIdentity, const Mat3& mat) override;
public:

	cTextRenderable(cGame *game, cFontShr font, std::string text = "", float textSize = 38.0f, Vec4 textColor = Vec4(1.0f)) : cRenderableWithShader(game, "resources/default.vs", "resources/default.ps")
	{
		this->font = font;
		this->text = text;
		this->textSize = textSize;
		this->textColor = textColor;
		setTextAllignment(TextAlignment::left);
		lengthDirty = true;
	}

	virtual ~cTextRenderable()
	{
		font = nullptr;
	}

	void setText(const std::string& text)
	{
		setText(text.c_str());
	}

	void setTextAllignment(TextAlignment alignment)
	{
		this->textAlignment = alignment;
	}

	void setText(const char* text)
	{
		this->text = text;
		lengthDirty = true;
	}

	void setTextSize(float size)
	{
		this->textSize = size;
		lengthDirty = true;
	}

	void setTextColor(const Vec4& color)
	{
		this->textColor = color;
	}
};