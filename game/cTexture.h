#pragma once

#include <SDL_image.h>
#include <GL/glew.h>
#include <stdio.h>

class cTexture
{
	GLuint gTexture;
	std::string name;

	IntVec2 dimensions;
	bool repeat;
public:
	cTexture() 
	{
		gTexture = -1;
	};

	cTexture(const std::string& fileName, bool repeat)
	{
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (surf == NULL)
		{
			printf("%s Error: \"%s\"\n", fileName.c_str(), SDL_GetError()); return;
		}

		GLenum data_fmt;
		if (surf->format->BytesPerPixel == 4)
		{
			data_fmt = GL_RGBA;
		}
		else if (surf->format->BytesPerPixel == 3)
		{
			data_fmt = GL_RGB;
		}
		else if (surf->format->BytesPerPixel == 1)
		{
			data_fmt = GL_RED;
		}
		else
		{
			assert(false);
		}
		glGenTextures(1, &gTexture);
		glBindTexture(GL_TEXTURE_2D, gTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, data_fmt, surf->w, surf->h, 0, data_fmt, GL_UNSIGNED_BYTE, surf->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		dimensions = IntVec2(surf->w, surf->h);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		SDL_FreeSurface(surf);

		name = fileName;
		this->repeat = repeat;

		printf("loading %s\n", name.c_str());
	}

	bool isRepeat() const
	{
		return repeat;
	}

	~cTexture()
	{

		printf("unloading %s\n", name.c_str());
		if (gTexture != -1)
		{
			glDeleteTextures(1, &gTexture);
		}
	}

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