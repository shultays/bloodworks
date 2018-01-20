#include "GroundRenderable.h"
#include "Bloodworks.h"
#include "cTexturedQuadRenderable.h"
#include "cShader.h"
#include "cGlobals.h"


GroundRenderable::GroundRenderable(Bloodworks *bloodworks) : cRenderable(bloodworks)
{
	this->bloodworks = bloodworks;

	GLint maxTextureSize = 1024;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

	cShaderShr shader = resources.getShader("resources/defaultWithUVScale.vs", "resources/default.ps");
	int uvBegin = shader->addUniform("uvBegin", TypeVec2).index;
	int uvSize = shader->addUniform("uvSize", TypeVec2).index;

	cTexturedQuadRenderable *bg = new cTexturedQuadRenderable(bloodworks, "", "");
	bg->setTexture(resources.getTexture(maxTextureSize >= 2048 ? "resources/ground/bg_base.png" : "resources/ground/bg_base_small.png", true));
	bg->setWorldMatrix(Mat3::scaleMatrix(2048.0f));
	bg->setShader(shader);
	bg->setUniform(uvBegin, Vec2(0.0f));
	bg->setUniform(uvSize, Vec2(4.0f));


	groundSize = 128;
	float t = max(bloodworks->getMapSize().x, bloodworks->getMapSize().y) + 50;
	while (groundSize < t)
	{
		groundSize += 128;
	}

	groundSize = min(maxTextureSize, groundSize);

	Vec2 mapSize = bloodworks->getMapSize() + 50.0f;

	int x = (int)ceil(mapSize.x / groundSize);
	int y = (int)ceil(mapSize.y / groundSize);


	int bufferCount = x * y;
	
	groundBuffers.resize(bufferCount);

	for (int z = 0; z < groundBuffers.size(); z++)
	{
		int a = z % x;
		int b = z / x;

		groundBuffers[z].xx = (-groundSize * x) / 2 + groundSize * a + groundSize / 2;
		groundBuffers[z].yy = (-groundSize * y) / 2 + groundSize * b + groundSize / 2;

		GL_CALL(glGenFramebuffers(1, &groundBuffers[z].frameBuffer));
		GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, groundBuffers[z].frameBuffer));

		GL_CALL(glGenTextures(1, &groundBuffers[z].frameBufferTexture));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, groundBuffers[z].frameBufferTexture));
		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, groundSize, groundSize, 0, GL_RGB, GL_UNSIGNED_BYTE, 0));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));

		GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, groundBuffers[z].frameBufferTexture, 0));

		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };


		GL_CALL(glDrawBuffers(1, DrawBuffers));
		Vec4 color = Vec4::fromColor(0xFFFFFFFF);
		glClearColor(color.r, color.g, color.b, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, groundBuffers[z].frameBuffer));
		GL_CALL(glViewport(0, 0, groundSize, groundSize));
		bloodworks->lastShader = nullptr;

		{
			cShaderShr shader = bg->getShader();
			shader->begin();

			shader->setViewMatrix(
				Mat3::translationMatrix(-groundSize * 0.5f - groundBuffers[z].xx, groundSize * 0.5f + groundBuffers[z].yy)
				.scaleBy(1.0f / groundSize, -1.0f / groundSize)
				.translateBy(0.5f)
				.scaleBy(2.0f));

			bloodworks->lastShader = shader;
			bloodworks->lastAlignment = bg->getAlignment();
			bg->setColor(Vec4(0.6f, 0.6f, 0.6f, 1.0f));
			bg->render(true, Mat3::identity(), AARect::invalid());
		}

		glBindTexture(GL_TEXTURE_2D, groundBuffers[z].frameBufferTexture);
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	{

		//glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);
		//bloodworks->lastShader = nullptr;
		//return;
	}
	cShaderShr brushShader = resources.getShader("resources/ground/brush");

	cTextureShr perlin = resources.getTexture("resources/ground/perlin.png", true);
	cTextureShr brush = resources.getTexture("resources/ground/brush.png", true);
	cTextureShr grass = resources.getTexture("resources/ground/grass.png", true);
	cTextureShr grass2 = resources.getTexture("resources/ground/grass2.png", true);
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


	int max = (int)(t * t) / 150000 + 20;

	float grassChance = 0.3f;
	float grass2Chance = 0.5f + grassChance;
	float stoneChance = 0.1f + grass2Chance;

	float totalChance = stoneChance;

	const AARect& rect = bloodworks->getMapLimits();

	for (int i = 0; i < max; i++)
	{
		float size = 0.4f;
		float imageSize = 0.9f;

		float colorMin = 0.6f;
		float colorMax = 0.9f;

		if (i < max * grassChance / totalChance)
		{
			brushRenderable->setTexture(2, grass);
			imageSize = 0.7f;
		}
		else if (i < max * grass2Chance / totalChance)
		{
			brushRenderable->setTexture(2, grass2);
			imageSize = 2.0f;
		}
		else if (i < max * stoneChance / totalChance)
		{
			brushRenderable->setTexture(2, stone);
			imageSize = 5.0f;
			colorMin = 0.9f;
		}

		brushShader->setUniform(uPerlinMat, Mat3::scaleMatrix(size).translateBy(randFloat(), randFloat()));

		brushShader->setUniform(uImageMat, Mat3::scaleMatrix(imageSize).translateBy(randFloat(), randFloat()));

		float renderSize = randFloat(160.0) + 160.0f;
		brushRenderable->setWorldMatrix(
			Mat3::scaleMatrix(renderSize, renderSize)
			.translateBy(rect.getRandomPos())
			.rotateBy(randFloat(pi_2))
		);

		brushShader->setUniform(uPerlinLevels, Vec2(randFloat(0.1f), randFloat(0.9f, 1.3f)));

		brushRenderable->setColor(Vec4(randFloat(colorMin, colorMax), randFloat(colorMin, colorMax), randFloat(colorMin, colorMax), randFloat(0.9f, 1.0f)));


		for (int z = 0; z < groundBuffers.size(); z++)
		{
			GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, groundBuffers[z].frameBuffer));

			shader->setViewMatrix(
				Mat3::translationMatrix(-groundSize * 0.5f - groundBuffers[z].xx, -groundSize * 0.5f + groundBuffers[z].yy)
				.scaleBy(1.0f / groundSize)
				.translateBy(0.5f)
				.scaleBy(2.0f));

			bloodworks->lastShader = shader;
			bloodworks->lastAlignment = brushRenderable->getAlignment();

			brushRenderable->render(true, Mat3::identity(), AARect::invalid());
		}
	}

	SAFE_DELETE(brushRenderable);

	for (int z = 0; z < groundBuffers.size(); z++)
	{
		glBindTexture(GL_TEXTURE_2D, groundBuffers[z].frameBufferTexture);
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);
	bloodworks->lastShader = nullptr;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	SAFE_DELETE(bg);
}

GroundRenderable::~GroundRenderable()
{
	for (int z = 0; z < groundBuffers.size(); z++)
	{
		glDeleteTextures(1, &groundBuffers[z].frameBufferTexture);
		glDeleteFramebuffers(1, &groundBuffers[z].frameBuffer);
	}
}

void GroundRenderable::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	bloodworks->lastShader = nullptr;
	cShaderShr shader = resources.getShader("resources/default");
	shader->begin();

	shader->setViewMatrix(bloodworks->getViewMatrix(RenderableAlignment::world));
	glBindBuffer(GL_ARRAY_BUFFER, defaultQuad);
	glBindVertexArray(quadBuffer);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);
	shader->setColor(Vec4(1.0f));
	shader->setTexture0(0);
	glActiveTexture(GL_TEXTURE0);
	for (int z = 0; z < groundBuffers.size(); z++)
	{
		glBindTexture(GL_TEXTURE_2D, groundBuffers[z].frameBufferTexture);
		shader->setWorldMatrix(Mat3::scaleMatrix(groundSize * 0.5f, groundSize * 0.5f).translateBy((float)groundBuffers[z].xx, (float)groundBuffers[z].yy));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	bloodworks->lastShader = nullptr;
}

