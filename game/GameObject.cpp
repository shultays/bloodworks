#include "GameObject.h"
#include "cRenderable.h"
#include "cRenderableContainer.h"
#include "cTexturedQuadRenderable.h"
#include "cTextRenderable.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "cFont.h"

void GameObject::updateMatrix()
{
	if (renderableGroup)
	{
		Mat3 mat = Mat3::scaleMatrix(scale).rotateBy(rotation).translateBy(pos);
		renderableGroup->setWorldMatrix(mat);
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

GameObject::RenderableData& GameObject::addTexture(const std::string& texture, const std::string& shader)
{
	checkRenderable();

	RenderableData renderableData;
	renderableData.id = bloodworks->getUniqueId();

	renderableData.type = RenderableDataType::texture;

	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable((cGame*)bloodworks, texture, shader.length() > 0 ? shader : "resources/default");
	renderable->setWorldMatrix(Mat3::identity());
	renderableData.renderable = renderable;
	renderableData.pos = Vec2::zero();
	renderableData.textureSize = renderable->getTexture()->getDimensions().toVec();
	renderableData.rotation = 0.0f;
	renderableData.color = 0xFFFFFFFF;
	renderableData.textSize = 0.0f;
	renderableData.alignment = alignment;
	renderableData.textAlignment = TextAlignment::center;
	renderableData.gameObject = this;

	renderableGroup->addRenderable(renderable);

	renderables.push_back(renderableData);

	renderableData.update();
	return renderables[(int)renderables.size() - 1];
}

GameObject::RenderableData& GameObject::addText(const std::string& text, const std::string& font)
{
	checkRenderable();

	RenderableData renderableData;
	renderableData.id = bloodworks->getUniqueId();
	renderableData.type = RenderableDataType::text;
	renderableData.pos = Vec2::zero();
	renderableData.textureSize = Vec2(10.0f);
	renderableData.rotation = 0.0f;
	renderableData.color = 0xFFFFFFFF;
	renderableData.textSize = 32.0f;
	renderableData.alignment = alignment;
	renderableData.textAlignment = TextAlignment::center;
	renderableData.gameObject = this;

	cTextRenderable *renderable = new cTextRenderable((cGame*)bloodworks, resources.getFont(font.size() ? font : "resources/fontData.txt"), text);
	renderableData.renderable = renderable;
	renderableGroup->addRenderable(renderable);
	updateMatrix();
	renderables.push_back(renderableData);

	renderableData.update();
	return renderables[(int)renderables.size() - 1];
}

void GameObject::removeRenderable(int id)
{
	for (int i = 0; i < renderables.size(); i++)
	{
		if (renderables[i].id == id)
		{
			renderableGroup->removeRenderable(renderables[i].renderable);
			SAFE_DELETE(renderables[i].renderable);
			renderables[i] = renderables[renderables.size() - 1];
			renderables.resize(renderables.size() - 1);
			return;
		}
	}
}

GameObject::RenderableData& GameObject::getRenderable(int id)
{
	for (int i = 0; i < renderables.size(); i++)
	{
		if (renderables[i].id == id)
		{
			return renderables[i];
		}
	}
	return renderables[0];
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


void GameObject::RenderableData::update()
{
	if (type == RenderableDataType::texture)
	{
		cTexturedQuadRenderable *quadRenderable = (cTexturedQuadRenderable*)renderable;
		quadRenderable->setColor(Vec4::fromColor(color));
		Mat3 mat = Mat3::scaleMatrix(textureSize).rotateBy(rotation).translateBy(pos);
		renderable->setWorldMatrix(mat);
		renderable->setAlignment(alignment);
	}
	else if (type == RenderableDataType::text)
	{
		cTextRenderable *textRenderable = (cTextRenderable*)renderable;
		textRenderable->setWorldMatrix(Mat3::translationMatrix(pos));
		textRenderable->setTextColor(Vec4::fromColor(color));
		textRenderable->setTextSize(textSize);
		textRenderable->setTextAllignment(textAlignment);
		textRenderable->setAlignment(alignment);
	}
}

