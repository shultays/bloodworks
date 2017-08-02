#pragma once

class Bloodworks;

#include "cRenderable.h"
#include <vector>

class cTexturedQuadRenderable;
class cRenderableWithShader;

#define USE_TEMP_BUFFER
#ifdef USE_TEMP_BUFFER
#define BUFFER_COUNT 2
#else
#define BUFFER_COUNT 1
#endif

class BloodRenderable : public cRenderable
{
	struct BodyPartData
	{
		cRenderableWithShader *renderable;
		bool isBlood;
		bool addedBlood;
		int toBeRemove;

		float time;
		Vec2 pos;
		float rotation;
		Vec2 size;
		Vec2 moveSpeed;

		float rotateSpeed;
		float initialScale;
	};

	std::list<BodyPartData> bodyParts;
	std::vector<cTextureShr> cachedBloods;
	cTextureShr bloodBg;
	cShaderShr bloodShader;
	cShaderShr defaultShader;
	Bloodworks *bloodworks;

	GLuint frameBuffer[BUFFER_COUNT];
	GLuint frameBufferTexture[BUFFER_COUNT];
	cShaderShr defaultPostProcessShader;

	Vec4 bloodColor;
public:
	BloodRenderable(Bloodworks *bloodworks);
	~BloodRenderable();
	void init();
	void addBlood(const Vec2& pos, const Vec2& moveSpeed, float size = 17.5f, std::list<BodyPartData>::iterator* insertPos = nullptr);
	void addBodyPart(cRenderableWithShader *partRenderable, const Vec2& pos, const Vec2& size, float angle, const Vec2& blowDir);

	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop) override;
	void tick();
	void reset();
};