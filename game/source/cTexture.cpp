#include "cTexture.h"

extern int totalResource;
extern bool hasError;

cTexture::cTexture()
{
	gTexture = -1;
	totalResource++;
}

cTexture::cTexture(const std::string& fileName, bool repeat)
{
	totalResource++;

	out << "loading " << name << "\n";
	dimensions.setZero();
	name = fileName;
	this->repeat = repeat;

	SDL_Surface* surf = IMG_Load(fileName.c_str());
	if (surf == NULL)
	{
		out << "surface_error: " << fileName << " " << SDL_GetError() << "\n";
		hasError = true;
		return;
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

}

cTexture::~cTexture()
{
	totalResource--;
#ifdef DEBUG
	out << "unloading " << name << "\n";
#endif
	if (gTexture != -1)
	{
		glDeleteTextures(1, &gTexture);
	}
}
