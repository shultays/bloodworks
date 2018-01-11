#pragma once

#include "cTools.h"
#include "cResources.h"
#include <string>
#include <GL/glew.h>

class cGame;
enum class RenderableAlignment;
extern GLuint defaultQuad;
extern GLuint quadBuffer;

class cRenderable
{
	friend class cGame;
	friend class cRenderableContainer;
	friend class cDebugRenderable;
protected:
	cGame *game;
	virtual void render()
	{
		render(true, Mat3::identity(), AARect::invalid());
	}
	Mat3 worldMatrix;
	Vec4 color;
	bool visible;
	RenderableAlignment alignment;

	int level;

	cRenderable *next;
	cRenderable *prev;
public:
	cRenderable(cGame *game);
	virtual ~cRenderable();

	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop = AARect::invalid()) = 0;

	bool isVisible() const
	{
		return visible;
	}

	RenderableAlignment getAlignment() const
	{
		return alignment;
	}

	void setVisible(bool visible)
	{
		this->visible = visible;
	}

	virtual void setAlignment(RenderableAlignment alignment)
	{
		this->alignment = alignment;
	}

	const Mat3& getWorldMatrix() const
	{
		return worldMatrix;
	}
	
	const Vec2& getPosition() const
	{
		return worldMatrix.row2().vec2();
	}

	void setWorldMatrix(const Mat3& worldMatrix);

	virtual void setColor(const Vec4& color)
	{
		this->color = color;
	}

	const Vec4& getColor() const
	{
		return color;
	}

	void setLevel(int level);

	int getLevel() const
	{
		return level;
	}

	virtual void setShader(const cShaderShr& shader){}
};
