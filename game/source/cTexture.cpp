#include "cTexture.h"
#include "coral.h"

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

	out << "loading " << fileName << "\n";
	dimensions.setZero();
	name = fileName;
	this->repeat = repeat;

	SDL_Surface* surf = IMG_Load(fileName.c_str());
	if (surf == NULL)
	{
		out << "surface_error: " << fileName << " " << SDL_GetError() << "\n";
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
	GL_CALL(glGenTextures(1, &gTexture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, gTexture));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, data_fmt, GL_UNSIGNED_BYTE, surf->pixels));
	if (hadGLError)
	{
		out << data_fmt << " " << surf->w << " " << surf->h << "\n";
	}
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	dimensions = IntVec2(surf->w, surf->h);

	glGenerateMipmap(GL_TEXTURE_2D);
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));

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
		GL_CALL(glDeleteTextures(1, &gTexture));
	}
}
