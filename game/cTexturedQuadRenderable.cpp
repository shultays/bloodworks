#include "cTexturedQuadRenderable.h"
#include "cShader.h"
#include "cTexture.h"
#include "cGlobals.h"

void cTexturedQuadRenderable::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	if (texture[0] == nullptr)
	{
		return;
	}

	cRenderableWithShader::render(isIdentity, mat, crop);

	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

	shader->setColor(color);
	shader->setTexture0(0);
	shader->setTexture1(1);
	shader->setTexture2(2);
	shader->setTexture3(3);

	for (int i = 3; i >= 0; i--)
	{
		if (texture[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			texture[i]->bindTexture();
		}
	}

	shader->setWorldMatrix(isIdentity ? worldMatrix : worldMatrix * mat);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);


	for (int i = 3; i >= 0; i--)
	{
		if (texture[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glActiveTexture(GL_TEXTURE0);
}

void cTexturedQuadRenderable::setTexture(int i, const std::string& texturePath)
{
	if (texturePath.length() == 0)
	{
		texture[i] = nullptr;
	}
	else
	{
		texture[i] = resources.getTexture(texturePath);
	}
}

cTexturedQuadRenderable::cTexturedQuadRenderable(cGame *game, const std::string& texturePath, const std::string& shaderPath) : cRenderableWithShader(game, shaderPath)
{
	setTexture(texturePath);
	if (texture[0] != nullptr)
	{
		setWorldMatrix(Mat3::translationMatrix(texture[0]->getDimensions().toVec()));
	}
}

cTexturedQuadRenderable::cTexturedQuadRenderable(cGame *game, const cTextureShr& texture, const std::string& shaderPath) : cRenderableWithShader(game, shaderPath)
{
	setTexture(texture);
	if (this->texture[0] != nullptr)
	{
		setWorldMatrix(Mat3::translationMatrix(this->texture[0]->getDimensions().toVec()));
	}
}

