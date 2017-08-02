#pragma once

#include "Sol.h"
#include "cMat.h"
#include "cVec.h"
#include "cVector.h"

class Bloodworks;
class cRenderable;
class cRenderableContainer;
enum class RenderableAlignment;
enum class TextAlignment;
class cParticle;
class cAnimatedTexturedQuadRenderable;
class cTexturedQuadRenderable;
class cTextRenderable;

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

	int id;
	bool toBeRemoved;

	sol::function onTick;
	sol::function onBulletCollision; // todo implement these
	sol::function onPlayerCollision;
	sol::function onMonsterCollision;

	std::string script;
	sol::table data;

	cRenderableContainer *renderableGroup;

	cVector<cTexturedQuadRenderable*> textureRenderables;
	cVector<cTextRenderable*> textRenderables;
	cVector<cParticle*> particles;
	cVector<cAnimatedTexturedQuadRenderable*> animations;
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

	cTexturedQuadRenderable* addTexture(const std::string& texture, const std::string& shader);
	cTextRenderable* addText(const std::string& text, const std::string& font);
	cParticle* addParticle(const std::string& particleTemplate, const sol::table& args);
	cAnimatedTexturedQuadRenderable* addAnimation(const std::string& name);

	void removeTexture(cTexturedQuadRenderable *texture);
	void removeText(cTextRenderable *text);
	void removeParticle(cParticle *particle);
	void removeAnimation(cAnimatedTexturedQuadRenderable *animation);
};
