#pragma once

#include "cFont.h"
#include "cRenderableWithShader.h"

class cTextRenderable : public cRenderableWithShader
{
private:
	cFontShr font;
	float textSize;
	std::string text;
	std::string textToPrint;
	friend class cDebugRenderable;
	bool lengthDirty;
	TextAlignment textAlignment;
	VerticalTextAlignment verticalTextAlignment;
	float maxLength;
	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;
	int maxLine;
	void strReplace(std::string& str, const std::string& oldStr, const std::string& newStr)
	{
		std::string::size_type pos = 0u;
		while ((pos = str.find(oldStr, pos)) != std::string::npos) 
		{
			str.replace(pos, oldStr.length(), newStr);
			pos += newStr.length();
		}
	}
public:
	cTextRenderable(cGame *game, cFontShr font, std::string text = "", float textSize = 38.0f, Vec4 textColor = Vec4(1.0f)) : cRenderableWithShader(game, "resources/default.vs", "resources/default.ps")
	{
		this->font = font;
		setText(text);
		this->textSize = textSize;
		this->color = textColor;
		setTextAlignment(TextAlignment::left);
		setVerticalTextAlignment(VerticalTextAlignment::bottom);
		lengthDirty = true;
		maxLength = FLT_MAX;
		maxLine = INT_MAX;
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

	void setTextAlignment(TextAlignment alignment)
	{
		this->textAlignment = alignment;
	}

	void setText(const std::string& text)
	{
		this->text = text;
		textToPrint = text;

		strReplace(textToPrint, "<br>", "\n");
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
	void setVerticalTextAlignment(VerticalTextAlignment param1);
	const std::string& getText() const;
	void setMaxLineCount(int maxLine)
	{
		this->maxLine = maxLine;
	}
};