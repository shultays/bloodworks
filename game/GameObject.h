#pragma once

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"

class Bloodworks;
class cRenderable;
class cRenderableContainer;
enum class RenderableAlignment;
enum class TextAlignment;

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
		int id;
		GameObject *gameObject;
		RenderableDataType type;
		cRenderable *renderable;
		int color;
		Vec2 pos;
		Vec2 textureSize;
		float rotation;

		float textSize;
		TextAlignment textAlignment;
		RenderableAlignment alignment;

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
	RenderableAlignment alignment;

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
	void setAlignment(RenderableAlignment alignment);
	void updateMatrix();
	void checkRenderable();
public:
	GameObject(Bloodworks *bloodworks);

	~GameObject();

	RenderableData& addTexture(const std::string& texture, const std::string& shader);
	RenderableData& addText(const std::string& text, const std::string& font);
	void removeRenderable(int id);

	RenderableData& getRenderable(int id);
};
