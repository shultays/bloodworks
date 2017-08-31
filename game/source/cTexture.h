#pragma once

#include <SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>

#include "cTools.h"

class cTexture
{
	GLuint gTexture;
	std::string name;

	IntVec2 dimensions;
	bool repeat;
public:
	cTexture();
	cTexture(const std::string& fileName, bool repeat);

	bool isRepeat() const
	{
		return repeat;
	}

	~cTexture();

	void bindTexture() const
	{
		glBindTexture(GL_TEXTURE_2D, gTexture);
	}

	const std::string& getName() const
	{
		return name;
	}

	const IntVec2& getDimensions() const
	{
		return dimensions;
	}

	void deleteSelf()
	{
		delete this;
	}
};