#include "cAnimatedRenderable.h"
#include "cShader.h"
#include "cTexture.h"

void cAnimatedTexturedQuadRenderable::render(bool isIdentity, const Mat3& mat)
{
	cRenderableWithShader::render(isIdentity, mat);

	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

	shader->setColor(color);


	currentAnimationTime += timer.getRender_dt();
	checkAnimationTime();

	cTextureShr texture = animations[currentAnimation].frames[currentAnimationFrame].texture;

	texture->bindTexture();
	shader->setWorldMatrix(worldMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);
}
