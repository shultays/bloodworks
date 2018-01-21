#include "BloodRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cShader.h"
#include "cTexturedQuadRenderable.h"
#include "cGlobals.h"
#include "cPostProcess.h"
#include <sstream>

void BloodRenderable::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	bloodworks->lastShader = nullptr;
	cShaderShr shader = defaultShader;
	shader->begin();
	glBindBuffer(GL_ARRAY_BUFFER, defaultQuad);
	glBindVertexArray(quadBuffer);
	shader->setViewMatrix(bloodworks->getViewMatrix(RenderableAlignment::world));

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);
	shader->setColor(Vec4(1.0f));
	shader->setTexture0(0);
	glActiveTexture(GL_TEXTURE0);

	for (int i = 0; i < bloodBuffers.size(); i++)
	{
		shader->setWorldMatrix(Mat3::scaleMatrix(bloodSize * 0.5f, -bloodSize * 0.5f).translateBy((float)bloodBuffers[i].x, (float)bloodBuffers[i].y));
		glBindTexture(GL_TEXTURE_2D, bloodBuffers[i].frameBufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glDisableVertexAttribArray(0);

	bloodworks->lastShader = nullptr;

	if (bodyParts.size() > 0)
	{
		std::list<BodyPartData>::iterator bodyPart = bodyParts.begin();
		while (bodyPart != bodyParts.end())
		{
			if (bodyPart->toBeRemove)
			{
				bodyPart++;
				continue;
			}
			float t = (timer.getRenderTime() - bodyPart->time) * 5.0f;

			if (bodyPart->isBlood)
			{
				if (t > 1.0f)
				{
					bodyPart->toBeRemove = 1;
				}
			}
			else
			{
				if (t > 1.9f)
				{
					bodyPart->toBeRemove = 1;
				}
				else if (t > 0.7f)
				{
					if (bodyPart->addedBlood == false)
					{
						bodyPart->addedBlood = true;
						addBlood(bodyPart->pos + bodyPart->moveSpeed * 1.0f + Vec2::fromAngle(randFloat(pi_2)) * randFloat(3.0f), Vec2::zero(), 3.0f + randFloat(5.0f), &bodyPart);
					}
				}
			}

			if (t >= 1.0f)
			{
				t = 1.0;
			}

			if (bodyPart->isBlood)
			{
				float scaleFactor = bodyPart->initialScale + t * (1.0f - bodyPart->initialScale);
				Mat3 mat = Mat3::scaleMatrix(scaleFactor * bodyPart->size.x, scaleFactor * bodyPart->size.y).rotateBy(bodyPart->rotation).translateBy(bodyPart->pos + bodyPart->moveSpeed * t);
				bodyPart->renderable->setWorldMatrix(mat);
			}
			else
			{
				float rotateAngle = bodyPart->rotation + t * bodyPart->rotateSpeed;
				Mat3 frame = Mat3::scaleMatrix(bodyPart->size * 1.0f)
					.rotateBy(rotateAngle)
					.translateBy(bodyPart->pos + bodyPart->moveSpeed * t);
				bodyPart->renderable->setWorldMatrix(frame);
			}

			bodyPart++;
		}

		{
			Vec2 windowSize = bloodworks->getScreenDimensions().toVec();
			float halfWidth = windowSize.w * 0.5f;
			float halfHeight = windowSize.h * 0.5f;

			glViewport(0, 0, bloodSize, bloodSize);

			bodyPart = bodyParts.begin();
			while (bodyPart != bodyParts.end())
			{
				bloodworks->lastShader = nullptr;
				cShaderShr shader = bodyPart->renderable->getShader();
				shader->begin();

				bloodworks->lastShader = shader;
				bloodworks->lastAlignment = bodyPart->renderable->getAlignment();

				if (bodyPart->toBeRemove == 1)
				{
					bodyPart->toBeRemove = 2;

					for (int z = 0; z < bloodBuffers.size(); z++)
					{
						shader->setViewMatrix(
							Mat3::translationMatrix(-bloodSize * 0.5f - bloodBuffers[z].x, -bloodSize * 0.5f - bloodBuffers[z].y)
							.scaleBy(1.0f / bloodSize)
							.translateBy(0.5f)
							.scaleBy(2.0f));

						glBindFramebuffer(GL_FRAMEBUFFER, bloodBuffers[z].frameBuffer);
						bodyPart->renderable->render(true, Mat3::identity(), crop);
					}
				}

				bodyPart++;
			}

			glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);
			bloodworks->lastShader = nullptr;
		}
		bloodworks->resetToBackBuffer();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	if (bodyParts.size() > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, coral.getTempFrameBuffer());

		glClearColor(bloodColor.r, bloodColor.g, bloodColor.b, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		bloodworks->lastShader = nullptr;

		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
			GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		std::list<BodyPartData>::iterator bodyPart = bodyParts.begin();
		while (bodyPart != bodyParts.end())
		{
			bodyPart->renderable->render(true, Mat3::identity(), crop);

			bodyPart++;
		}


		bloodworks->resetToBackBuffer();
	
		defaultPostProcess->bind();

		glBindBuffer(GL_ARRAY_BUFFER, postProcessQuad);
		glBindVertexArray(postProcessVertex);

		defaultPostProcess->getShader()->bindUV(0, 0);
		glBindTexture(GL_TEXTURE_2D, coral.getTempFrameBufferTexture());

		defaultPostProcess->getShader()->setTexture0(0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	bloodworks->lastShader = nullptr;
}

void BloodRenderable::tick()
{
	LastEntrySet S("BloodRenderable::tick");
	if (bloodworks->isFirstTick())
	{
		out << "BloodRenderable::tick\n";
	}
	std::list<BodyPartData>::iterator bodyPart = bodyParts.begin();
	while (bodyPart != bodyParts.end())
	{
		if (bodyPart->toBeRemove)
		{
			SAFE_DELETE(bodyPart->renderable);
			bodyPart = bodyParts.erase(bodyPart);
			continue;
		}
		bodyPart++;
	}
}

void BloodRenderable::reset()
{
	std::list<BodyPartData>::iterator bodyPart = bodyParts.begin();
	while (bodyPart != bodyParts.end())
	{
		SAFE_DELETE(bodyPart->renderable);
		bodyPart++;
	}
	bodyParts.clear();


	glClearColor(bloodColor.r, bloodColor.g, bloodColor.b, 0.0f);
	for (int z = 0; z < bloodBuffers.size(); z++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloodBuffers[z].frameBuffer);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	game->resetToBackBuffer();
}

BloodRenderable::BloodRenderable(Bloodworks *bloodworks) : cRenderable((cGame*)bloodworks)
{
	this->bloodworks = bloodworks;
	bloodColor = Vec4::fromColor(0xFF660000);
}

BloodRenderable::~BloodRenderable()
{
	reset();
	for (auto& t : cachedBloods)
	{
		t = nullptr;
	}
	cachedBloods.clear();
	bloodShader = nullptr;
	defaultShader = nullptr;
	bloodBg = nullptr;
	
	delete defaultPostProcess;

	for (int i = 0; i < bloodBuffers.size(); i++)
	{
		glDeleteTextures(1, &bloodBuffers[i].frameBufferTexture);
		glDeleteFramebuffers(1, &bloodBuffers[i].frameBuffer);
	}
}


void BloodRenderable::init()
{
	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "resources/blood/blood" << i << ".png";
		cachedBloods.push_back(resources.getTexture(ss.str(), true));
	}


	bloodSize = 128;
	float t = max(bloodworks->getMapSize().x, bloodworks->getMapSize().y) + 50;
	while (bloodSize < t)
	{
		bloodSize += 128;
	}

	GLint maxTextureSize = 1024;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	
	bloodSize = min(maxTextureSize, bloodSize);

	Vec2 mapSize = bloodworks->getMapSize() + 50.0f;

	int x = (int)ceil(mapSize.x / bloodSize);
	int y = (int)ceil(mapSize.y / bloodSize);

	bloodBg = resources.getTexture("resources/blood/blood_bg.png", true);

	bloodShader = resources.getShader("resources/blood/blood.vs", "resources/blood/blood.ps");
	defaultShader = resources.getShader("resources/default.vs", "resources/default.ps");


	defaultPostProcess = new cPostProcess();
	defaultPostProcess->init(bloodworks, resources.getShader("resources/post_process/default.vs", "resources/post_process/default.ps"), -1);

	cVector<GLuint> textures;
	cVector<GLuint> buffers;

	int bufferCount = x * y;
	buffers.resize(bufferCount);
	textures.resize(bufferCount);

	GL_CALL(glGenFramebuffers(bufferCount, &buffers[0]));
	GL_CALL(glGenTextures(bufferCount, &textures[0]));


	for (int i = 0; i < bufferCount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, buffers[i]);

		GL_CALL(glBindTexture(GL_TEXTURE_2D, textures[i]));
		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bloodSize, bloodSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

		GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0));

		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

		GL_CALL(glDrawBuffers(1, DrawBuffers));
		glClearColor(bloodColor.r, bloodColor.g, bloodColor.b, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

	}
	bloodworks->resetToBackBuffer();


	bloodBuffers.resize(bufferCount);
	for (int i = 0; i < bufferCount; i++)
	{
		int a = i % x;
		int b = i / x;
		bloodBuffers[i].frameBuffer = buffers[i];
		bloodBuffers[i].frameBufferTexture = textures[i];

		bloodBuffers[i].x = (-bloodSize * x) / 2 + bloodSize * a + bloodSize / 2;
		bloodBuffers[i].y = (-bloodSize * y) / 2 + bloodSize * b + bloodSize / 2;
	}


	CHECK_GL_ERROR_X("blood renderable init");
}

void BloodRenderable::addBlood(const Vec2& pos, const Vec2& moveSpeed, float size, std::list<BodyPartData>::iterator* insertPos)
{
	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, cachedBloods[randInt((int)cachedBloods.size())], "resources/blood/blood");
	renderable->setWorldMatrix(Mat3::scaleMatrix(randFloat(8.0f, 14.0f)).translateBy(pos));
	renderable->setTexture(1, bloodBg);
	renderable->setColor(bloodColor);

	BodyPartData data;
	data.isBlood = true;
	data.renderable = renderable;
	data.time = timer.getTime();
	data.pos = pos;
	data.size = (randFloat() * 0.8f + 1.5f) * size;
	data.rotation = randFloat(pi_2);
	data.moveSpeed = moveSpeed;

	data.rotateSpeed = 0.0f;
	data.addedBlood = false;
	data.toBeRemove = 0;
	data.initialScale = 0.5f;

	if (insertPos)
	{
		data.renderable->setWorldMatrix(Mat3::scaleMatrix(randFloat(0.01f, 0.01f)));
		data.initialScale = 0.0f;
		bodyParts.insert(bodyParts.begin(), data);
	}
	else
	{
		bodyParts.push_back(data);
	}
}

void BloodRenderable::addBodyPart(cRenderableWithShader *partRenderable, const Vec2& pos, const Vec2& size, float angle, const Vec2& blowDir)
{
	BodyPartData bodyPartData;
	bodyPartData.isBlood = false;
	bodyPartData.renderable = partRenderable;
	bodyPartData.pos = pos;
	bodyPartData.size = size;
	bodyPartData.rotation = angle;
	bodyPartData.moveSpeed = blowDir * Mat2::rotation(randFloat(-0.9f, 0.9f)) * randFloat(0.7f, 1.5f);
	bodyPartData.time = timer.getTime();
	bodyPartData.rotateSpeed = randFloat(1.4f, 2.2f);
	bodyPartData.addedBlood = false;
	bodyPartData.toBeRemove = 0;
	bodyPartData.initialScale = 1.0f;
	if (randBool())
	{
		bodyPartData.rotateSpeed = -bodyPartData.rotateSpeed;
	}

	bodyParts.push_back(bodyPartData);
}
