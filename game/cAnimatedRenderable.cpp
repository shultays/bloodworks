#include "cAnimatedRenderable.h"
#include "cShader.h"
#include "cTexture.h"

void cAnimatedTexturedQuadRenderable::render()
{
	cRenderableWithShader::render();

	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);

	shader->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));


	currentAnimationTime += timer.getRender_dt();
	checkAnimationTime();

	cTextureShr texture = animations[currentAnimation].frames[currentAnimationFrame].texture;

	texture->bindTexture();
	shader->setWorldMatrix(worldMatrix);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);
}

