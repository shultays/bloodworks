#include "cTextRenderable.h"
#include "cTexture.h"
#include "cShader.h"

void cTextRenderable::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	cRenderableWithShader::render(isIdentity, mat, crop);
	font->texture->bindTexture();
	int row = 0;
	int lastCharToDraw = 0;
	while (true)
	{
		Mat3 temp2 = worldMatrix;
		temp2 = Mat3::scaleMatrix(textSize) * temp2;
		temp2.translateBy((float)font->leftPadding, (float)font->bottomPadding);

		if (verticalTextAlignment == VerticalTextAlignment::top)
		{
			temp2.translateBy(0.0f, -textSize);
		}
		else if (verticalTextAlignment == VerticalTextAlignment::mid)
		{
			temp2.translateBy(0.0f, -textSize * 0.5f);
		}

		int firstCharToDraw = lastCharToDraw;
		//if (lengthDirty) // todo fix this
		{
			lengthDirty = false;
			length = 0.0f;

			int lastSpaceIndex = -1;

			for (int i = firstCharToDraw; i < textToPrint.size(); i++)
			{
				float charSize = font->defaultSize;
				if (font->charInfos[textToPrint[i]].x >= 0)
				{
					charSize = (float)font->charInfos[textToPrint[i]].w;
				}
				if (textToPrint[i] == ' ')
				{
					lastSpaceIndex = i;
				}
				if (textToPrint[i] == '\n')
				{
					lastCharToDraw = i + 1;
					break;
				}
				float t = charSize * textSize / font->maxWidth + font->leftPadding + font->rightPadding;
				float newLength = length + t * mat._00;
				if (newLength > maxLength)
				{
					if (lastSpaceIndex == -1)
					{
						lastCharToDraw = i;
					}
					else
					{
						lastCharToDraw = lastSpaceIndex + 1;
					}
					break;
				}
				length = newLength;
				lastCharToDraw = i + 1;
			}
		}

		Mat3 temp = isIdentity ? temp2 : temp2 * mat;

		if (textAlignment == TextAlignment::center)
		{
			temp.translateBy(-length * 0.5f, 0.0f);
		}
		else if (textAlignment == TextAlignment::right)
		{
			temp.translateBy(-length, 0.0f);
		}

		if (row > 0)
		{
			temp.translateBy(0.0f, -row * (textSize + 3.0f));
		}
		
		glActiveTexture(GL_TEXTURE0);
		shader->setColor(color);
		shader->setTexture0(0);

		for (int i = firstCharToDraw; i < lastCharToDraw; i++)
		{
			float charSize = font->defaultSize;
			auto& info = font->charInfos[textToPrint[i]];
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
		}

		glDisableVertexAttribArray(0);
		if (multiline == false || lastCharToDraw >= textToPrint.size())
		{
			break;
		}
		row++;
	}

	glDisable(GL_TEXTURE_2D);
}

void cTextRenderable::setVerticalTextAllignment(VerticalTextAlignment verticalTextAlignment)
{
	this->verticalTextAlignment = verticalTextAlignment;
}

const std::string& cTextRenderable::getText() const
{
	return text;
}

void cTextRenderable::setMultiline(bool multiline)
{
	this->multiline = multiline;
}
