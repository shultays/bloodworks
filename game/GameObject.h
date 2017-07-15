#pragma once

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"

class Bloodworks;
class cRenderable;
class cRenderableContainer;

class GameObject
{
	friend class MissionController;
	friend class BloodworksLuaWorld;
	Bloodworks *bloodworks;

	enum RenderableDataType
	{
		texture,
		text
	};

	struct RenderableData
	{
		GameObject *gameObject;
		RenderableDataType type;
		cRenderable *renderable;
		int color;
		Vec2 pos;
		Vec2 textureSize;
		float rotation;

		float textSize;
		int textAlignment;
		int alignment;

		void update();
	};

	int id;
	bool toBeRemoved;
	bool hasOnTick;
	bool hasBulletCollision;
	bool hasPlayerCollision;
	bool hasMonsterCollision;

	std::string script;
	sol::table data;

	cRenderableContainer *renderableGroup;
	std::vector<RenderableData> renderables;
	int level;

	Vec2 pos;
	Vec2 scale;
	float rotation;

	void setPosition(const Vec2& pos)
	{
		this->pos = pos;
		updateMatrix();
	}
	void setRotation(float rotation)
	{
		this->rotation = rotation;
		updateMatrix();
	}
	void setScale(const Vec2& scale)
	{
		this->scale = scale;
		updateMatrix();
	}

	void setLevel(int level);

	void updateMatrix();

public:
	GameObject(Bloodworks *bloodworks);

	~GameObject();

	RenderableData& addTexture(const std::string& texture, const std::string& shader);

	RenderableData& addText(const std::string& text, const std::string& font);

	RenderableData& getRenderable(int i)
	{
		return renderables[i];
	}
private:
	void checkRenderable();
};
