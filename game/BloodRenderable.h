#pragma once

class Bloodworks;

#include "cRenderable.h"
#include <vector>

class BloodRenderable : public cRenderable
{
	struct BloodData
	{
		cTexturedQuadRenderable *renderable;
		float time;
		Vec2 pos;
		float rotation;
		float size;
		Vec2 moveSpeed;
	};
	struct BodyPartData
	{
		cRenderable *renderable;
		Vec2 pos;
		Vec2 size;
		float rotation;
		Vec2 moveSpeed;
		Vec2 rotatePoint;
		float time;
		float timeScale;
		float rotateSpeed;
	};

	std::vector<BodyPartData> bodyParts;
	std::vector<BloodData> bloods;
	std::vector<cTextureShr> cachedBloods;
	cShaderShr bloodShader;
	cShaderShr defaultShader;
	Bloodworks *bloodworks;

	GLuint frameBuffer;
	GLuint frameBufferTexture;
public:
	BloodRenderable(Bloodworks *bloodworks);
	~BloodRenderable();
	void init();
	void addBlood(const Vec2& pos, const Vec2& moveSpeed, float size = 17.5f);
	void addBodyPart(cRenderable *partRenderable, const Vec2& pos, const Vec2& size, float angle, const Vec2& rotatePoint, const Vec2& blowDir);

	virtual void render(bool isIdentity, const Mat3& mat) override;
	void tick();

};