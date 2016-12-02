#include "GameObject.h"
#include "cRenderable.h"
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

GameObject::~GameObject()
{
	SAFE_DELETE(renderableGroup);
}

GameObject::RenderableData& GameObject::addTexture(const std::string& texture, const std::string& shader)
{
	checkRenderable();

	RenderableData renderableData;

	renderableData.type = RenderableDataType::texture;

	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable((cGame*)bloodworks, texture.c_str(), shader.length() > 0 ? shader.c_str() : "resources/default");
	renderable->setWorldMatrix(Mat3::identity());
	renderableData.renderable = renderable;
	renderableData.pos = Vec2::zero();
	renderableData.textureSize = renderable->getTexture()->getDimensions().toVec();
	renderableData.rotation = 0.0f;
	renderableData.color = 0xFFFFFFFF;
	renderableData.textSize = 0.0f;
	renderableData.alignment = 0;
	renderableData.textAlignment = 0;
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

	renderableData.type = RenderableDataType::text;
	renderableData.pos = Vec2::zero();
	renderableData.textureSize = Vec2(10.0f);
	renderableData.rotation = 0.0f;
	renderableData.color = 0xFFFFFFFF;
	renderableData.textSize = 32.0f;
	renderableData.alignment = 0;
	renderableData.textAlignment = 1;
	renderableData.gameObject = this;

	cTextRenderable *renderable = new cTextRenderable((cGame*)bloodworks, resources.getFont(font.size() ? font.c_str() : "resources/fontData.txt"), text);
	renderableData.renderable = renderable;
	renderableGroup->addRenderable(renderable);
	updateMatrix();
	renderables.push_back(renderableData);

	renderableData.update();
	return renderables[(int)renderables.size() - 1];
}

void GameObject::checkRenderable()
{
	if (renderableGroup == nullptr)
	{
		renderableGroup = new cRenderableGroup(bloodworks);
		renderableGroup->setWorldMatrix(Mat3::translationMatrix(Vec2::zero()));
		bloodworks->addRenderable(renderableGroup, level);
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
		renderable->setAlignment((RenderableAlignment)alignment);
	}
	else if (type == RenderableDataType::text)
	{
		cTextRenderable *textRenderable = (cTextRenderable*)renderable;
		textRenderable->setWorldMatrix(Mat3::translationMatrix(pos));
		textRenderable->setTextColor(Vec4::fromColor(color));
		textRenderable->setTextSize(textSize);
		textRenderable->setTextAllignment((TextAlignment)textAlignment);
		textRenderable->setAlignment((RenderableAlignment)alignment);
	}
}

