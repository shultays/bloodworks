#pragma once

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"

class Bloodworks;
class cRenderable;
class cRenderableContainer;
enum class RenderableAlignment;
enum class TextAlignment;
class cParticle;
class cAnimatedTexturedQuadRenderable;

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

	struct RenderableData // todo is this necessary? we can just return instances of texture/text
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

	sol::function onTick;
	sol::function onBulletCollision; // todo implement these
	sol::function onPlayerCollision;
	sol::function onMonsterCollision;

	std::string script;
	sol::table data;

	cRenderableContainer *renderableGroup;
	std::vector<RenderableData> renderables;
	std::vector<cParticle*> particles;
	std::vector<cAnimatedTexturedQuadRenderable*> animations;
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

	const Vec2& getPosition() const
	{
		return pos;
	}

	float getRotation() const
	{
		return rotation;
	}

	const Vec2& getScale() const
	{
		return scale;
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
	cParticle* addParticle(const std::string& particleTemplate, const sol::table& args);
	cAnimatedTexturedQuadRenderable* addAnimation(const std::string& name);
	void removeRenderable(int id);

	RenderableData& getRenderable(int id);
};
