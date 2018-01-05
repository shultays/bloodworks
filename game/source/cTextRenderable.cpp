#include "cTextRenderable.h"
#include "cTexture.h"
#include "cShader.h"

void cTextRenderable::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	cRenderableWithShader::render(isIdentity, mat, crop);
	font->texture->bindTexture();

	struct LineData
	{
		int startIndex;
		int endIndex;
		float length;
		bool useSurplus;
	};

	cVector<struct LineData> lineData;
	lineData.reserve(4);
	float scaledLength = maxLength * mat._00;

	{
		int lastCharToDraw = 0;

		while (true)
		{
			struct LineData data;
			data.useSurplus = false;

			int firstCharToDraw = lastCharToDraw;
			int lastSpaceIndex = -1;
			float lengthAtSpace = 0.0f;
			float length = 0.0f;
			for (int i = firstCharToDraw; i < textToPrint.size(); i++)
			{
				float charSize = font->defaultSize;
				unsigned char c = textToPrint[i];
				if (font->charInfos[c].x >= 0)
				{
					charSize = (float)font->charInfos[c].w;
				}
				if (c == ' ')
				{
					lastSpaceIndex = i;
					lengthAtSpace = length;
				}
				if (c == '\n')
				{
					lastCharToDraw = i + 1;
					break;
				}
				float t = charSize * textSize / font->maxWidth + font->leftPadding + font->rightPadding;
				float newLength = length + t * mat._00;
				if (newLength > scaledLength)
				{
					if (lastSpaceIndex == -1)
					{
						lastCharToDraw = i;
					}
					else
					{
						lastCharToDraw = lastSpaceIndex + 1;
						length = lengthAtSpace;
						data.useSurplus = true;
					}
					break;
				}
				length = newLength;
				lastCharToDraw = i + 1;
			}

			data.startIndex = firstCharToDraw;
			data.endIndex = lastCharToDraw;
			data.length = length;
			lineData.push_back(data);
			if (lineData.size() >= maxLine || lastCharToDraw >= textToPrint.size())
			{
				break;
			}
		}
	}

	float lineHeight = (textSize + linePadding);

	for (int l=0; l<lineData.size(); l++)
	{
		auto& line = lineData[l];
		Mat3 temp2 = worldMatrix;
		temp2 = Mat3::scaleMatrix(textSize) * temp2;
		temp2.translateBy((float)font->leftPadding, (float)font->bottomPadding);

		if (verticalTextAlignment == VerticalTextAlignment::top)
		{
			temp2.translateBy(0.0f, -textSize - lineHeight*lineData.size());
		}
		else if (verticalTextAlignment == VerticalTextAlignment::mid)
		{
			temp2.translateBy(0.0f, textSize * -0.5f + lineHeight * ((lineData.size() - 1) * 0.5f));
		}

		Mat3 temp = isIdentity ? temp2 : temp2 * mat;

		if (textAlignment == TextAlignment::center)
		{
			temp.translateBy(-line.length * 0.5f, 0.0f);
		}
		else if (textAlignment == TextAlignment::right)
		{
			temp.translateBy(-line.length, 0.0f);
		}

		float subplusLength = 0.0f;

		if (textAlignment == TextAlignment::fit && line.endIndex - line.startIndex > 0 && line.useSurplus)
		{
			if (scaledLength - line.length < scaledLength * 0.5f)
			{
				subplusLength = (scaledLength - line.length) / (line.endIndex - line.startIndex);
			}
		}

		if (l > 0)
		{
			temp.translateBy(0.0f, -l * lineHeight * mat._00);
		}
		
		glActiveTexture(GL_TEXTURE0);
		shader->setColor(color * textColor);
		shader->setTexture0(0);

		for (int i = line.startIndex; i < line.endIndex; i++)
		{
			unsigned char c = textToPrint[i];
			float charSize = font->defaultSize;
			auto& info = font->charInfos[c];
			if (info.x >= 0)
			{
				shader->setWorldMatrix(temp);

				glBindBuffer(GL_ARRAY_BUFFER, info.vbo);

				shader->bindPosition(sizeof(float) * 8, 0);
				shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
				shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				charSize = (float)info.w;
			}
			Vec2 shift = Vec2(charSize * textSize / font->maxWidth + font->leftPadding + font->rightPadding, 0.0f);
			shift = (Vec3(shift.x, shift.y, 0.0f) * mat).vec2;
			temp.translateBy(shift);
			if (subplusLength > 0.0f)
			{
				temp.translateBy(subplusLength, 0.0f);
			}
		}
	}

	glDisableVertexAttribArray(0);
	glDisable(GL_TEXTURE_2D);
}

void cTextRenderable::setVerticalTextAlignment(VerticalTextAlignment verticalTextAlignment)
{
	this->verticalTextAlignment = verticalTextAlignment;
}

const std::string& cTextRenderable::getText() const
{
	return text;
}