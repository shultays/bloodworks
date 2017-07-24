#pragma once

#include "cFont.h"
#include "cRenderableWithShader.h"

class cTextRenderable : public cRenderableWithShader
{
private:
	cFontShr font;
	float textSize;
	std::string text;
	friend class cDebugRenderable;
	float length;
	bool lengthDirty;
	TextAlignment textAlignment;
	VerticalTextAlignment verticalTextAlignment;
	float maxLength;
	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;
public:

	cTextRenderable(cGame *game, cFontShr font, std::string text = "", float textSize = 38.0f, Vec4 textColor = Vec4(1.0f)) : cRenderableWithShader(game, "resources/default.vs", "resources/default.ps")
	{
		this->font = font;
		this->text = text;
		this->textSize = textSize;
		this->color = textColor;
		setTextAllignment(TextAlignment::left);
		setVerticalTextAllignment(VerticalTextAlignment::bottom);
		lengthDirty = true;
		maxLength = FLT_MAX;
	}

	virtual ~cTextRenderable()
	{
		font = nullptr;
	}

	void setMaxLength(float maxLength)
	{
		this->maxLength = maxLength;
		lengthDirty = true;
	}

	float getMaxLength() const
	{
		return maxLength;
	}

	void setTextAllignment(TextAlignment alignment)
	{
		this->textAlignment = alignment;
	}

	void setText(const std::string& text)
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
		this->color = color;
	}
	void setVerticalTextAllignment(VerticalTextAlignment param1);
	const std::string& getText() const;
};