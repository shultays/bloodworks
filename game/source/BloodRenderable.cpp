#include "BloodRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cShader.h"
#include "cTexturedQuadRenderable.h"
#include "cGlobals.h"
#include <sstream>

void BloodRenderable::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	bloodworks->lastShader = nullptr;
	glEnable(GL_TEXTURE_2D);
	cShaderShr shader = defaultShader;
	shader->begin();
	shader->setViewMatrix(bloodworks->getViewMatrix(RenderableAlignment::world));
	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);
	shader->setColor(Vec4(1.0f));
	shader->setTexture0(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture[0]);
	shader->setWorldMatrix(Mat3::scaleMatrix(bloodSize * 0.5f, -bloodSize * 0.5f));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);
	bloodworks->lastShader = nullptr;

	if (bodyParts.size() > 0)
	{
#ifndef USE_TEMP_BUFFER
		cVector<BodyPartData*> toRender;
#endif
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

#ifndef USE_TEMP_BUFFER
			if (bodyPart->toBeRemove)
			{
				toRender.push_back(&(*bodyPart));
			}

			bodyPart->renderable->render(true, Mat3::identity(), crop);
#endif
			bodyPart++;
		}

#ifndef USE_TEMP_BUFFER
		if (toRender.size())
#endif
		{
#ifdef USE_TEMP_BUFFER
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[1]);
			glClearColor(bloodColor.r, bloodColor.g, bloodColor.b, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
#else
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[0]);
#endif

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

				shader->setViewMatrix(
					Mat3::translationMatrix(-bloodSize * 0.5f, -bloodSize * 0.5f)
					.scaleBy(1.0f / bloodSize)
					.translateBy(0.5f)
					.scaleBy(2.0f));

				bloodworks->lastShader = shader;
				bloodworks->lastAlignment = bodyPart->renderable->getAlignment();

#ifdef USE_TEMP_BUFFER
				if (bodyPart->toBeRemove == 1)
				{
					bodyPart->toBeRemove = 2;

					glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[0]);
					bodyPart->renderable->render(true, Mat3::identity(), crop);
					glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[1]);
				}
#endif
				bodyPart->renderable->render(true, Mat3::identity(), crop);

				bodyPart++;
			}

			glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);
			bloodworks->lastShader = nullptr;
		}
		bloodworks->resetToBackBuffer();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef USE_TEMP_BUFFER

	if (bodyParts.size() > 0)
	{
		bloodworks->resetToBackBuffer();

		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
			GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		bloodworks->lastShader = nullptr;
		glEnable(GL_TEXTURE_2D);
		shader = defaultShader;
		shader->begin();
		shader->setViewMatrix(bloodworks->getViewMatrix(RenderableAlignment::world));
		glBindBuffer(GL_ARRAY_BUFFER, quad);

		shader->bindPosition(sizeof(float) * 8, 0);
		shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
		shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);
		shader->setColor(Vec4(1.0f));
		shader->setTexture0(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture[1]);
		shader->setWorldMatrix(Mat3::scaleMatrix(bloodSize * 0.5f, -bloodSize * 0.5f));
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glDisableVertexAttribArray(0);

		glDisable(GL_TEXTURE_2D);
	}

#endif
	bloodworks->lastShader = nullptr;
}

void BloodRenderable::tick()
{
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

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[0]);

	glClearColor(bloodColor.r, bloodColor.g, bloodColor.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

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
	for(auto& t : cachedBloods)
	{
		t = nullptr;
	}
	cachedBloods.clear();
	bloodShader = nullptr;
	defaultShader = nullptr;
	bloodBg = nullptr;
	glDeleteTextures(BLOOD_BUFFER_COUNT, frameBufferTexture);
	glDeleteFramebuffers(BLOOD_BUFFER_COUNT, frameBuffer);
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
	bloodBg = resources.getTexture("resources/blood/blood_bg.png", true);
	
	bloodShader = resources.getShader("resources/blood/blood.vs", "resources/blood/blood.ps");
	defaultShader = resources.getShader("resources/default.vs", "resources/default.ps");
	defaultPostProcessShader = resources.getShader("resources/post_process/default.vs", "resources/post_process/default.ps");

	glGenFramebuffers(BLOOD_BUFFER_COUNT, frameBuffer);
	glGenTextures(BLOOD_BUFFER_COUNT, frameBufferTexture);

	for (int i = 0; i < BLOOD_BUFFER_COUNT; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, frameBufferTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bloodSize, bloodSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameBufferTexture[i], 0);

		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

		glDrawBuffers(1, DrawBuffers);
		glClearColor(bloodColor.r, bloodColor.g, bloodColor.b, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

	}
	bloodworks->resetToBackBuffer();
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
