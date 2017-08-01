#include "BloodRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cShader.h"
#include "cTexturedQuadRenderable.h"
#include "cGlobals.h"
#include <sstream>

#define blood_size (2048)

//#define USE_TEMP_BUFFER
// todo USE_TEMP_BUFFER should render things to a blood texture and then draw it all

void BloodRenderable::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
#ifdef USE_TEMP_BUFFER
	glBindFramebuffer(GL_FRAMEBUFFER, coral.getTempFrameBuffer());

	glClearColor(bloodColor.r, bloodColor.g, bloodColor.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
#endif

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
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	shader->setWorldMatrix(Mat3::scaleMatrix(blood_size * 0.5f, -blood_size * 0.5f));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);
	bloodworks->lastShader = nullptr;

	if (bodyParts.size() > 0)
	{
		std::vector<BodyPartData*> toRender;
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
					bodyPart->toBeRemove = true;
				}
			}
			else
			{
				if (t > 1.8f)
				{
					bodyPart->toBeRemove = true;
				}
				else if (t > 0.6f)
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
				float scaleFactor = 0.5f + t * 0.5f;
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

			if (bodyPart->toBeRemove)
			{
				toRender.push_back(&(*bodyPart));
			}

			bodyPart->renderable->render(true, Mat3::identity(), crop);

			bodyPart++;
		}

		if (toRender.size())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

			Vec2 windowSize = bloodworks->getScreenDimensions().toVec();
			float halfWidth = windowSize.w * 0.5f;
			float halfHeight = windowSize.h * 0.5f;

			glViewport(0, 0, blood_size, blood_size);

			for (auto& bodyPart : toRender)
			{
				bloodworks->lastShader = nullptr;
				cShaderShr shader = bodyPart->renderable->getShader();
				shader->begin();

				shader->setViewMatrix(
					Mat3::translationMatrix(-blood_size * 0.5f, -blood_size * 0.5f)
					.scaleBy(1.0f / blood_size)
					.translateBy(0.5f)
					.scaleBy(2.0f));

				bloodworks->lastShader = shader;
				bloodworks->lastAlignment = bodyPart->renderable->getAlignment();

				bodyPart->renderable->render(true, Mat3::identity(), crop);
				bodyPart->renderable->setVisible(false);
			}

			glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);
			bloodworks->lastShader = nullptr;
		}
		bloodworks->resetToBackBuffer();
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef USE_TEMP_BUFFER
	bloodworks->resetToBackBuffer();
	glBindTexture(GL_TEXTURE_2D, coral.getTempFrameBufferTexture());
	defaultPostProcessShader->begin();
	glBindBuffer(GL_ARRAY_BUFFER, postProcessQuad);

	defaultPostProcessShader->bindUV(0, 0);
	defaultPostProcessShader->setTexture0(0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
#endif
	bloodworks->lastShader = nullptr;
}

void BloodRenderable::tick()
{
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

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

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
	glDeleteTextures(1, &frameBufferTexture);
	glDeleteFramebuffers(1, &frameBuffer);
}


void BloodRenderable::init()
{
	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "resources/blood/blood" << i << ".png";
		cachedBloods.push_back(resources.getTexture(ss.str(), true));
	}
	bloodBg = resources.getTexture("resources/blood/blood_bg.png", true);
	
	bloodShader = resources.getShader("resources/blood/blood.vs", "resources/blood/blood.ps");
	defaultShader = resources.getShader("resources/default.vs", "resources/default.ps");
	defaultPostProcessShader = resources.getShader("resources/post_process/default.vs", "resources/post_process/default.ps");

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, blood_size, blood_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameBufferTexture, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };


	glDrawBuffers(1, DrawBuffers);
	Vec4 color = Vec4::fromColor(0xFF660000);
	glClearColor(color.r, color.g, color.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	data.toBeRemove = false;

	if (insertPos)
	{
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
	bodyPartData.toBeRemove = false;
	if (randBool())
	{
		bodyPartData.rotateSpeed = -bodyPartData.rotateSpeed;
	}

	bodyParts.push_back(bodyPartData);
}
