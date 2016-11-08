#include "BloodRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"


#include <sstream>

void BloodRenderable::render()
{
	for(int i=0; i<bloods.size(); i++)
	{
		auto& blood = bloods[i];
		float scaleFactor = (timer.getTime() - blood.time) * 5.0f;
		bool remove = false;
		if (scaleFactor > 1.0f)
		{
			scaleFactor = 1.0;
			remove = true;
		}

		Mat3 mat = Mat3::scaleMatrix(Vec2(17.0f) * scaleFactor * blood.scale).rotateBy(blood.rotation).translateBy(blood.pos);
		blood.renderable->setWorldMatrix(mat);

		if (remove)
		{
			restingBloods.push_back(blood.renderable);
			bloods[i] = bloods[(int)bloods.size() - 1];
			bloods.resize(bloods.size() - 1);
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
	for (auto& t : restingBloods)
	{
		SAFE_DELETE(t);
	}
	restingBloods.clear();
	cachedShader = nullptr;
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
}

void BloodRenderable::addBlood(const Vec2& pos)
{
	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, cachedBloods[randInt((int)cachedBloods.size())]->getName(), "resources/blood/blood");
	renderable->setSize(10.0f);
	renderable->setPosition(pos);
	renderable->setTexture(1, "resources/blood/blood_bg.png");
	renderable->setColor(Vec4::fromColor(0xFF880000));
	bloodworks->addRenderable(renderable, 2);

	BloodData data;
	data.renderable = renderable;
	data.time = timer.getTime();
	data.pos = pos;
	data.scale = randFloat() * 0.6f + 0.9f;
	data.rotation = randFloat(pi_2);
	bloods.push_back(data);
}
