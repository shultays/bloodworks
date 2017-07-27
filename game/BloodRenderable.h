#pragma once

class Bloodworks;

#include "cRenderable.h"
#include <vector>

class cTexturedQuadRenderable;
class cRenderableWithShader;

class BloodRenderable : public cRenderable
{
	struct BodyPartData
	{
		cRenderableWithShader *renderable;
		bool isBlood;
		float time;
		Vec2 pos;
		float rotation;
		Vec2 size;
		Vec2 moveSpeed;

		float rotateSpeed;
		bool addedBlood;
		bool toBeRemove;
	};

	std::list<BodyPartData> bodyParts;
	std::vector<cTextureShr> cachedBloods;
	cTextureShr bloodBg;
	cShaderShr bloodShader;
	cShaderShr defaultShader;
	Bloodworks *bloodworks;

	GLuint frameBuffer;
	GLuint frameBufferTexture;
public:
	BloodRenderable(Bloodworks *bloodworks);
	~BloodRenderable();
	void init();
	void addBlood(const Vec2& pos, const Vec2& moveSpeed, float size = 17.5f, std::list<BodyPartData>::iterator* insertPos = nullptr);
	void addBodyPart(cRenderableWithShader *partRenderable, const Vec2& pos, const Vec2& size, float angle, const Vec2& blowDir);

	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;
	void tick();
	void reset();
};