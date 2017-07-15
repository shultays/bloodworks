#include "cTextRenderable.h"
#include "cTexture.h"
#include "cShader.h"

void cTextRenderable::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	cRenderableWithShader::render(isIdentity, mat, crop);
	font->texture->bindTexture();
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

	//if (lengthDirty) // todo fix this
	{
		lengthDirty = false;
		length = 0.0f;

		for (int i = 0; i < text.size(); i++)
		{
			float charSize = font->defaultSize;
			if (font->charInfos[text[i]].x >= 0)
			{
				charSize = (float)font->charInfos[text[i]].w;
			}
			float t = charSize * textSize / font->maxWidth + font->leftPadding + font->rightPadding;
			length += t * mat._00;
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


	glActiveTexture(GL_TEXTURE0);
	shader->setColor(color);
	shader->setTexture0(0);

	for (int i = 0; i < text.size(); i++)
	{
		float charSize = font->defaultSize;
		auto& info = font->charInfos[text[i]];
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
