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
	cFont(const char *fontData)
	{
		this->fontData = fontData;
		for (int i = 0; i < 128; i++)
		{
			charInfos[i].x = -1;
			charInfos[i].w = 32;
		}

		std::ifstream ifs(fontData, std::ifstream::in);

		std::string fontImagePath;
		ifs >> fontImagePath;

		ifs >> leftPadding >> rightPadding >> topPadding >> bottomPadding;

		texture = resources.getTexture(fontImagePath.c_str());

		Vec2 size = texture->getDimensions().toVec();
		maxWidth = maxHeight = -1;
		int t = 0;
		while (ifs.good())
		{
			char c;
			int x, y, w, h;
			ifs >> c >> x >> y >> w >> h;
			t++;
			charInfos[c].x = x;
			charInfos[c].y = y;
			charInfos[c].w = w;
			charInfos[c].h = h;

			maxWidth = max(maxWidth, w);
			maxHeight = max(maxHeight, h);
		}

		for (int i = 0; i < 128; i++)
		{
			if (charInfos[i].x < 0)
			{
				continue;
			}

			float pad = 0.0015f; //todo remove this

			GLfloat vertexData[] =
			{
				0.0f, charInfos[i].h / (float)maxHeight,
				charInfos[i].x / size.w + pad, charInfos[i].y / size.h + pad,
				1.0f, 1.0f, 1.0f, 1.0f,

				charInfos[i].w / (float)maxWidth, charInfos[i].h / (float)maxHeight,
				(charInfos[i].x + charInfos[i].w) / size.w - pad, charInfos[i].y / size.h + pad,
				1.0f, 1.0f, 1.0f, 1.0f,

				charInfos[i].w / (float)maxWidth,  0.0f,
				(charInfos[i].x + charInfos[i].w) / size.w - pad, (charInfos[i].y + charInfos[i].h) / size.h - pad,
				1.0f, 1.0f, 1.0f, 1.0f,

				0.0f, 0.0f, 
				charInfos[i].x / size.w + pad, (charInfos[i].y + charInfos[i].h) / size.h - pad,
				1.0f, 1.0f, 1.0f, 1.0f,
			};

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

			charInfos[i].vbo = vbo;
		}
	}

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
	virtual void render() override
	{
		shader->setColor(textColor);
		font->texture->bindTexture();
		Mat3 mat = worldMatrix;
		mat.row0.x *= textSize;
		mat.row1.y *= textSize;
		mat.row2.vec2 += Vec2((float)font->leftPadding, (float)font->bottomPadding);

		for (int i = 0; i < text.size(); i++)
		{
			if (font->charInfos[text[i]].x >= 0)
			{
				shader->setWorldMatrix(mat);

				glBindBuffer(GL_ARRAY_BUFFER, font->charInfos[text[i]].vbo);

				shader->bindPosition(sizeof(float) * 8, 0);
				shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
				shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}

			mat.translateBy((float)font->charInfos[text[i]].w * textSize / font->maxWidth + font->leftPadding + font->rightPadding, 0.0f);
		}

		glDisableVertexAttribArray(0);

		glDisable(GL_TEXTURE_2D);
	}

public:
	cTextRenderable(cGame *game, cFontShr font, std::string text = "", float textSize = 38.0f, Vec4 textColor = Vec4(1.0f)) : cRenderableWithShader(game, "resources/default.vs", "resources/default.ps")
	{
		this->font = font;
		this->text = text;
		this->textSize = textSize;
		this->textColor = textColor;
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