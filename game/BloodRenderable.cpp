#include "BloodRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cShader.h"
#include <sstream>

GLuint FramebufferName = 0;
GLuint renderedTexture;

#define blood_size 2048

void BloodRenderable::render(bool isIdentity, const Mat3& mat)
{
	for (int i = 0; i < bodyParts.size(); i++)
	{
		auto& bodyPart = bodyParts[i];
		if (bodyPart.renderable->isVisible() == false)
		{
			continue;
		}
		float t = (timer.getTime() - bodyPart.time) * bodyPart.timeScale * 5.0f;
		bool remove = false;
		if (t >= 1.0f)
		{
			t = 1.0;
			remove = true;
		}

		Mat3 frame = Mat3::scaleMatrix(bodyPart.size).
			translateBy(bodyPart.rotatePoint).rotateBy(bodyPart.rotation + t * bodyPart.rotateSpeed).
			translateBy(bodyPart.pos + bodyPart.moveSpeed * t);
		bodyPart.renderable->setWorldMatrix(frame);

		if (remove)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			cShaderShr shader = resources.getShader("resources/default.vs", "resources/default.ps");

			Vec2 windowSize = bloodworks->getScreenDimensions().toVec();
			float halfWidth = windowSize.w * 0.5f;
			float halfHeight = windowSize.h * 0.5f;

			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
				GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			shader->setViewMatrix(
				Mat3::translationMatrix(-blood_size * 0.5f, -blood_size * 0.5f)
				.scaleBy(1.0f / blood_size)
				.translateBy(0.5f)
				.scaleBy(2.0f));
			bloodworks->lastShader = shader;

			shader->begin();
			glViewport(0, 0, blood_size, blood_size);
			bodyPart.renderable->render(true, Mat3::identity());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			bodyPart.renderable->setVisible(false);

			bloodworks->lastShader = nullptr;
			glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	for(int i=0; i<bloods.size(); i++)
	{
		auto& blood = bloods[i];
		if (blood.renderable->isVisible() == false)
		{
			continue;
		}
		float scaleFactor = (timer.getTime() - blood.time) * 6.0f;
		bool remove = false;
		if (scaleFactor >= 1.0f)
		{
			scaleFactor = 1.0;
			remove = true;
		}
		float moveFactor = scaleFactor;
		scaleFactor = 0.5f + scaleFactor * 0.5f;
		Mat3 mat = Mat3::scaleMatrix(scaleFactor * blood.size).rotateBy(blood.rotation).translateBy(blood.pos + blood.moveSpeed * moveFactor);
		blood.renderable->setWorldMatrix(mat);

		if (remove)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			cShaderShr shader = bloodRenderShader;
			shader->begin();
			blood.renderable->setShader(shader);

			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
				GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			shader->setViewMatrix(
				Mat3::translationMatrix(-blood_size * 0.5f, -blood_size * 0.5f)
				.scaleBy(1.0f / blood_size)
				.translateBy(0.5f)
				.scaleBy(2.0f));
			bloodworks->lastShader = shader;

			glViewport(0, 0, blood_size, blood_size);
			blood.renderable->render(true, Mat3::identity());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			blood.renderable->setVisible(false);

			bloodworks->lastShader = nullptr;
			glViewport(0, 0, bloodworks->getScreenDimensions().w, bloodworks->getScreenDimensions().h);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	bloodworks->lastShader = nullptr;
	glEnable(GL_TEXTURE_2D);
	cShaderShr shader = resources.getShader("resources/default.vs", "resources/default.ps");
	shader->begin();
	shader->setViewMatrix(bloodworks->getViewMatrix());
	glBindBuffer(GL_ARRAY_BUFFER, quad);

	shader->bindPosition(sizeof(float) * 8, 0);
	shader->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	shader->bindColor(sizeof(float) * 8, sizeof(float) * 4);
	shader->setColor(Vec4(1.0f));
	shader->setUniform("uTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	shader->setWorldMatrix(Mat3::scaleMatrix(blood_size * 0.5f, -blood_size * 0.5f));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);
	bloodworks->lastShader = nullptr;

}

void BloodRenderable::tick()
{
	for (int i = 0; i < bloods.size(); i++)
	{
		if (bloods[i].renderable->isVisible() == false)
		{
			SAFE_DELETE(bloods[i].renderable);
			bloods[i] = bloods[bloods.size() - 1];
			bloods.resize(bloods.size() - 1);
			i--;
		}
	}
	for (int i = 0; i < bodyParts.size(); i++)
	{
		if (bodyParts[i].renderable->isVisible() == false)
		{
			SAFE_DELETE(bodyParts[i].renderable);
			bodyParts[i] = bodyParts[bodyParts.size() - 1];
			bodyParts.resize(bodyParts.size() - 1);
			i--;
		}
	}
}

BloodRenderable::BloodRenderable(Bloodworks *bloodworks) : cRenderable((cGame*)bloodworks)
{
	this->bloodworks = bloodworks;
}

BloodRenderable::~BloodRenderable()
{
	for(auto& t : cachedBloods)
	{
		t = nullptr;
	}
	cachedBloods.clear();
	for (auto& t : bloods)
	{
		SAFE_DELETE(t.renderable);
	}
	bloods.clear();
	for (auto& t : bodyParts)
	{
		SAFE_DELETE(t.renderable);
	}
	bodyParts.clear();
	cachedShader = nullptr;
	bloodRenderShader = nullptr;
}


void BloodRenderable::init()
{
	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "resources/blood/blood" << i << ".png";
		cachedBloods.push_back(resources.getTexture(ss.str().c_str()));
	}
	cachedShader = resources.getShader("resources/blood/blood.vs", "resources/blood/blood.ps");
	bloodRenderShader = resources.getShader("resources/blood/bloodToTexture.vs", "resources/blood/bloodToTexture.ps");

	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, blood_size, blood_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };


	glDrawBuffers(1, DrawBuffers);
	Vec4 color = Vec4::fromColor(0xFF660000);
	glClearColor(color.r, color.g, color.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		int a = 5;
	}
}

void BloodRenderable::addBlood(const Vec2& pos, const Vec2& moveSpeed, float size)
{
	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, cachedBloods[randInt((int)cachedBloods.size())]->getName(), "resources/blood/blood");
	renderable->setWorldMatrix(Mat3::scaleMatrix(randFloat(8.0f, 14.0f)).translateBy(pos));
	renderable->setTexture(1, "resources/blood/blood_bg.png");
	renderable->setColor(Vec4::fromColor(0xFF660000));
	bloodworks->addRenderable(renderable, 2);

	BloodData data;
	data.renderable = renderable;
	data.time = timer.getTime();
	data.pos = pos;
	data.size = (randFloat() * 0.8f + 1.5f) * size;
	data.rotation = randFloat(pi_2);
	data.moveSpeed = moveSpeed;
	bloods.push_back(data);
}

void BloodRenderable::addBodyPart(cRenderable *partRenderable, const Vec2& pos, const Vec2& size, float angle, const Vec2& rotatePoint, const Vec2& blowDir)
{
	BodyPartData bodyPartData;
	bodyPartData.renderable = partRenderable;
	bodyPartData.pos = pos;
	bodyPartData.size = size;
	bodyPartData.rotation = angle;
	bodyPartData.moveSpeed = blowDir * Mat2::rotation(randFloat(-0.9f, 0.9f)) * randFloat(0.7f, 1.5f);
	bodyPartData.time = timer.getTime();
	bodyPartData.timeScale = randFloat(0.8f, 1.2f);
	bodyPartData.rotateSpeed = randFloat(1.4f, 2.2f);
	bodyPartData.rotatePoint = rotatePoint;
	if (randBool())
	{
		bodyPartData.rotateSpeed = -bodyPartData.rotateSpeed;
	}
	bloodworks->addRenderable(partRenderable, 3);

	bodyParts.push_back(bodyPartData);
}