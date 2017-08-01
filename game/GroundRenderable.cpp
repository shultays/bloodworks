#include "GroundRenderable.h"
#include "Bloodworks.h"
#include "cTexturedQuadRenderable.h"
#include "cShader.h"
#include "cGlobals.h"

#define GROUND_SIZE 2048

GroundRenderable::GroundRenderable(Bloodworks *bloodworks) : cRenderable(bloodworks)
{

	this->bloodworks = bloodworks;

	cShaderShr shader = resources.getShader("resources/defaultWithUVScale.vs", "resources/default.ps");
	int uvBegin = shader->addUniform("uvBegin", TypeVec2).index;
	int uvSize = shader->addUniform("uvSize", TypeVec2).index;

	cTexturedQuadRenderable *bg = new cTexturedQuadRenderable(bloodworks, "", "");
	bg->setTexture(resources.getTexture("resources/ground/bg_base.png", true));
	bg->setWorldMatrix(Mat3::scaleMatrix(2048.0f));
	bg->setShader(shader);
	bg->setUniform(uvBegin, Vec2(0.0f));
	bg->setUniform(uvSize, Vec2(4.0f));


	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GROUND_SIZE, GROUND_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameBufferTexture, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };


	glDrawBuffers(1, DrawBuffers);
	Vec4 color = Vec4::fromColor(0xFFFFFFFF);
	glClearColor(color.r, color.g, color.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, GROUND_SIZE, GROUND_SIZE);
	bloodworks->lastShader = nullptr;

	{
		cShaderShr shader = bg->getShader();
		shader->begin();

		shader->setViewMatrix(
			Mat3::translationMatrix(-GROUND_SIZE * 0.5f, -GROUND_SIZE * 0.5f)
			.scaleBy(1.0f / GROUND_SIZE)
			.translateBy(0.5f)
			.scaleBy(2.0f));

		bloodworks->lastShader = shader;
		bloodworks->lastAlignment = bg->getAlignment();
		bg->setColor(Vec4(0.6f, 0.6f, 0.6f, 1.0f));
		bg->render(true, Mat3::identity(), Rect::invalid());
	}

	SAFE_DELETE(bg);

	cShaderShr brushShader = resources.getShader("resources/ground/brush");

	cTextureShr perlin = resources.getTexture("resources/ground/perlin.png", true);
	cTextureShr brush = resources.getTexture("resources/ground/brush.png", true);
	cTextureShr grass = resources.getTexture("resources/ground/grass.png", true);
	cTextureShr grass2 = resources.getTexture("resources/ground/grass2.png", true);
	cTextureShr earth = resources.getTexture("resources/ground/earth.png", true);
	cTextureShr stone = resources.getTexture("resources/ground/stone.png", true);

	int uPerlinMat = brushShader->addUniform("uPerlinMat", TypeMat3).index;
	int uImageMat = brushShader->addUniform("uImageMat", TypeMat3).index;
	int uPerlinLevels = brushShader->addUniform("uPerlinLevels", TypeVec2).index;

	cTexturedQuadRenderable *brushRenderable = new cTexturedQuadRenderable(bloodworks, "", "");
	brushRenderable->setTexture(0, brush);
	brushRenderable->setTexture(1, perlin);
	brushRenderable->setTexture(2, grass);
	brushRenderable->setShader(brushShader);

	shader = brushRenderable->getShader();
	shader->begin();


	int max = 100;

	for (int i = 0; i < max; i++)
	{
		float size = 0.4f;
		float imageSize = 0.9f;

		float colorMin = 0.6f;
		float colorMax = 0.9f;

		if (i < max * 0.1f)
		{
			brushRenderable->setTexture(2, grass);
			imageSize = 0.7f;
		}
		else if (i < max * 0.16f)
		{
			brushRenderable->setTexture(2, grass2);
			imageSize = 2.0f;
		}
		else if (i < max * 0.22)
		{
			brushRenderable->setTexture(2, stone);
			imageSize = 5.0f;
			colorMin = 0.9f;
		}
		else
		{
			imageSize = 1.0f;
			brushRenderable->setTexture(2, grass2);
		}

		brushShader->setUniform(uPerlinMat, Mat3::scaleMatrix(size).translateBy(randFloat(), randFloat()));

		brushShader->setUniform(uImageMat, Mat3::scaleMatrix(imageSize).translateBy(randFloat(), randFloat()));

		float renderSize = randFloat(160.0) + 160.0f;
		brushRenderable->setWorldMatrix(
			Mat3::scaleMatrix(renderSize, renderSize)
			.translateBy(
				randFloat(bloodworks->getMapMin().x, bloodworks->getMapMax().x),
				randFloat(bloodworks->getMapMin().y, bloodworks->getMapMax().y))
			.rotateBy(randFloat(pi_2))
		);

		brushShader->setUniform(uPerlinLevels, Vec2(randFloat(0.1f), randFloat(0.9f, 1.3f)));

		brushRenderable->setColor(Vec4(randFloat(colorMin, colorMax), randFloat(colorMin, colorMax), randFloat(colorMin, colorMax), randFloat(0.9f, 1.0f)));

		shader->setViewMatrix(
			Mat3::translationMatrix(-GROUND_SIZE * 0.5f, -GROUND_SIZE * 0.5f)
			.scaleBy(1.0f / GROUND_SIZE)
			.translateBy(0.5f)
			.scaleBy(2.0f));
		
		bloodworks->lastShader = shader;
		bloodworks->lastAlignment = brushRenderable->getAlignment();

		brushRenderable->render(true, Mat3::identity(), Rect::invalid());

	}


	SAFE_DELETE(brushRenderable);

	glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);
	bloodworks->lastShader = nullptr;

}

GroundRenderable::~GroundRenderable()
{
	glDeleteTextures(1, &frameBufferTexture);
	glDeleteFramebuffers(1, &frameBuffer);
}

void GroundRenderable::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{

	bloodworks->lastShader = nullptr;
	glEnable(GL_TEXTURE_2D);
	cShaderShr shader = resources.getShader("resources/default");
	shader->begin();
	shader->setViewMatrix(bloodworks->getViewMatrix(RenderableAlignment::world));
	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);
	shader->setColor(Vec4(1.0f));
	shader->setTexture0(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	shader->setWorldMatrix(Mat3::scaleMatrix(GROUND_SIZE * 0.5f, -GROUND_SIZE * 0.5f));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);
	bloodworks->lastShader = nullptr;

}

