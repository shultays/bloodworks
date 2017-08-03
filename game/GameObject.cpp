#include "GameObject.h"
#include "cRenderable.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "cTextRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cFont.h"
#include "cParticle.h"
#include "cAnimatedRenderable.h"
#include "cAnimationTemplate.h"
#include "CollisionController.h"
#include "cCircle.h"
#include "cRect.h"
#include "cCapsule.h"

void GameObject::updateMatrix()
{
	if (renderableGroup)
	{
		Mat3 mat = Mat3::scaleMatrix(scale).rotateBy(rotation).translateBy(pos);
		renderableGroup->setWorldMatrix(mat);
	}

	Mat2 rot = Mat2::rotation(rotation);
	float s = scale.length();
	for (auto& collider : colliders)
	{
		if (collider.t == ColliderData::type_circle)
		{
			Vec2 t = pos + ((collider.v0 * scale) * rot);
			float r = collider.f * s;
			bloodworks->getCollisionController()->relocateCollider(collider.id, Circle(t, r));
		}
		else if (collider.t == ColliderData::type_capsule)
		{
			Vec2 t0 = pos + ((collider.v0 * scale) * rot);
			Vec2 t1 = pos + ((collider.v1 * scale) * rot);
			float r = collider.f * s;
			bloodworks->getCollisionController()->relocateCollider(collider.id, Capsule(t0, t1, r));
		}
		else if (collider.t == ColliderData::type_rect)
		{
			Vec2 t0 = pos + ((collider.v0 * scale) * rot);
			Vec2 t1 = scale * collider.v1;
			float r = collider.f + rotation;
			bloodworks->getCollisionController()->relocateCollider(collider.id, Rect(t0, t1, r));
		}
	}
}

GameObject::GameObject(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	renderableGroup = nullptr;
	toBeRemoved = false;
	id = bloodworks->getUniqueId();
	alignment = RenderableAlignment::world;

	pos.setZero();
	scale = Vec2(1.0f);
	rotation = 0.0f;
	level = GAME_OBJECTS;
}

void GameObject::setLevel(int level)
{
	this->level = level;
	if (renderableGroup)
	{
		renderableGroup->setLevel(level);
	}
}

void GameObject::setAlignment(RenderableAlignment alignment)
{
	this->alignment = alignment;
	if (renderableGroup)
	{
		renderableGroup->setAlignment(alignment);
	}
}

GameObject::~GameObject()
{
	SAFE_DELETE(renderableGroup);
}

cTexturedQuadRenderable* GameObject::addTexture(const std::string& texture, const std::string& shader)
{
	checkRenderable();

	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable((cGame*)bloodworks, texture, shader.length() > 0 ? shader : "resources/default");
	renderable->setWorldMatrix(Mat3::identity());
	renderableGroup->addRenderable(renderable);

	return renderable;
}

cTextRenderable* GameObject::addText(const std::string& text, const std::string& font)
{
	checkRenderable();

	cTextRenderable *renderable = new cTextRenderable((cGame*)bloodworks, resources.getFont(font.size() ? font : "resources/fontData.txt"), text);
	renderableGroup->addRenderable(renderable);

	return renderable;
}

cParticle* GameObject::addParticle(const std::string& particleTemplate, const sol::table& args)
{
	checkRenderable();
	cParticle* p = new cParticle(bloodworks, bloodworks->getParticleTemplate(particleTemplate), args);
	renderableGroup->addRenderable(p);
	particles.push_back(p);
	return p;
}

cAnimatedTexturedQuadRenderable* GameObject::addAnimation(const std::string& name)
{
	checkRenderable();
	cAnimationTemplate *animationTemplate = bloodworks->getAnimationTemplate(name);
	cAnimatedTexturedQuadRenderable* animatedQuadRenderable = new cAnimatedTexturedQuadRenderable(bloodworks, animationTemplate->getShader());
	animatedQuadRenderable->addAnimation(animationTemplate);
	animatedQuadRenderable->setWorldMatrix(Mat3::scaleMatrix(animationTemplate->getSize()));
	renderableGroup->addRenderable(animatedQuadRenderable);
	return animatedQuadRenderable;
}

void GameObject::removeTexture(cTexturedQuadRenderable *texture)
{
	textureRenderables.swapToTailRemoveElement(texture);
	renderableGroup->removeRenderable(texture);
	SAFE_DELETE(texture);
}

void GameObject::removeText(cTextRenderable *text)
{
	textRenderables.swapToTailRemoveElement(text);
	renderableGroup->removeRenderable(text);
	SAFE_DELETE(text);
}

void GameObject::removeParticle(cParticle *particle)
{
	particles.swapToTailRemoveElement(particle);
	renderableGroup->removeRenderable(particle);
	SAFE_DELETE(particle);
}

void GameObject::removeAnimation(cAnimatedTexturedQuadRenderable *animation)
{
	animations.swapToTailRemoveElement(animation);
	renderableGroup->removeRenderable(animation);
	SAFE_DELETE(animation);
}

int GameObject::addCircleCollider(const Vec2& shift, float radius)
{
	ColliderData& collider = colliders.insertAndGetReference();
	collider.t = ColliderData::type_circle;
	collider.v0 = shift;
	collider.f = radius;

	Vec2 t = pos + ((shift * scale) * Mat2::rotation(rotation));
	float r = radius * scale.length();
	collider.id = bloodworks->getCollisionController()->addCollider(Circle(t, r), this);
	return collider.id;
}

int GameObject::addCapsuleCollider(const Vec2& pos0, const Vec2& pos1, float radius)
{
	ColliderData& collider = colliders.insertAndGetReference();
	collider.t = ColliderData::type_capsule;
	collider.v0 = pos0;
	collider.v1 = pos1;
	collider.f = radius;

	Vec2 t0 = pos + ((pos0 * scale) * Mat2::rotation(rotation));
	Vec2 t1 = pos + ((pos1 * scale) * Mat2::rotation(rotation));
	float r = radius * scale.length();
	collider.id = bloodworks->getCollisionController()->addCollider(Capsule(t0, t1, r), this);
	return collider.id;
}

int GameObject::addRectCollider(const Vec2& shift, const Vec2& size, float rotation)
{
	ColliderData& collider = colliders.insertAndGetReference();
	collider.t = ColliderData::type_capsule;
	collider.v0 = shift;
	collider.v1 = size;
	collider.f = rotation;

	Vec2 t0 = pos + ((shift * scale) * Mat2::rotation(this->rotation));
	Vec2 t1 = scale * size;
	float r = rotation + this->rotation;
	collider.id = bloodworks->getCollisionController()->addCollider(Rect(t0, t1, r), this);
	return collider.id;
}

void GameObject::checkRenderable()
{
	if (renderableGroup == nullptr)
	{
		renderableGroup = new cRenderableContainer(bloodworks);
		renderableGroup->setWorldMatrix(Mat3::translationMatrix(Vec2::zero()));
		bloodworks->addRenderable(renderableGroup, level);
		renderableGroup->setAlignment(alignment);
		updateMatrix();
	}
}

