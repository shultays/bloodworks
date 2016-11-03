#pragma once

#include <SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>

class cTexture
{
	GLuint gTexture;
	std::string name;

	IntVec2 dimensions;
public:
	cTexture(){};

	cTexture(const char *fileName)
	{
		SDL_Surface* surf = IMG_Load(fileName);
		if (surf == NULL)
		{
			printf("%s Error: \"%s\"\n", fileName, SDL_GetError()); return;
		}

		GLenum data_fmt = GL_RGBA;
		glGenTextures(1, &gTexture);
		glBindTexture(GL_TEXTURE_2D, gTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, data_fmt, GL_UNSIGNED_BYTE, surf->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		dimensions = IntVec2(surf->w, surf->h);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		SDL_FreeSurface(surf);

		name = fileName;
	}

	void bindTexture() const
	{
		glBindTexture(GL_TEXTURE_2D, gTexture);
	}

	const char* getName() const
	{
		return name.c_str();
	}

	const IntVec2 getDimensions()
	{
		return dimensions;
	}

	void deleteSelf()
	{
		delete this;
	}
};