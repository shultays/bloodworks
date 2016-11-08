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
		float scale;
	};
protected:
	virtual void render() override;

	std::vector<BloodData> bloods;
	std::vector<cTexturedQuadRenderable*> restingBloods;
	std::vector<cTextureShr> cachedBloods;
	cShaderShr cachedShader;
	Bloodworks *bloodworks;
public:
	BloodRenderable(Bloodworks *bloodworks);
	~BloodRenderable();
	void init();
	void addBlood(const Vec2& pos);
};