#include "cResources.h"
#include "cFont.h"
#include "cTexture.h"
#include "cShader.h"

void cResources::objectFreed(cFont* object)
{
	std::string ID = fontId(object->getFontDataPath());
	std::unordered_map<std::string, cFontShr>::iterator got = fonts.find(ID);
	assert(got != fonts.end());
	got->second.setCustomDeallocator(nullptr);
	fonts.erase(got);
}

void cResources::objectFreed(cTexture* object)
{
	std::string ID = textureID(object->getName());
	std::unordered_map<std::string, cTextureShr>::iterator got = textures.find(ID);
	assert(got != textures.end());
	got->second.setCustomDeallocator(nullptr);
	textures.erase(got);
}

void cResources::objectFreed(cShader* object)
{
	std::string ID = shaderID(object->getVertexShaderFileName(), object->getPixelShaderFileName());
	std::unordered_map<std::string, cShaderShr>::iterator got = shaders.find(ID);
	assert(got != shaders.end());
	got->second.setCustomDeallocator(nullptr);
	shaders.erase(got);
}

std::string cResources::shaderID(const std::string& vertexShaderFile, const std::string& pixelShaderFile) const
{
	std::string ID = vertexShaderFile;
	ID += pixelShaderFile;
	return ID;
}

std::string cResources::textureID(const std::string& textureName) const
{
	std::string ID = textureName;
	return ID;
}

std::string cResources::fontId(const std::string& textureName) const
{
	std::string ID = textureName;
	return ID;
}

void cResources::deleteObject(cShader* object)
{
	delete object;
}

void cResources::deleteObject(cFont* object)
{
	delete object;
}

void cResources::deleteObject(cTexture* object)
{
	delete object;
}

cResources::cResources()
{

}

cResources::~cResources()
{
	freeAll();
}

cShaderShr cResources::getShader(const std::string& file)
{
	return getShader(file + ".vs", file + ".ps");
}

cShaderShr cResources::getShader(const std::string& vertexShaderFile, const std::string& pixelShaderFile)
{
	std::string ID = shaderID(vertexShaderFile, pixelShaderFile);
	std::unordered_map<std::string, cShaderShr>::iterator got = shaders.find(ID);
	if (got != shaders.end())
	{
		return got->second;
	}

	cShaderShr shader = new cShader();
	shader.setCustomDeallocator(this);

	shader->loadFromFile(vertexShaderFile, pixelShaderFile);
	shaders[ID] = shader;
	return shader;
}

cTextureShr cResources::getTexture(const std::string& textureName)
{
	std::string ID = textureID(textureName);
	std::unordered_map<std::string, cTextureShr>::iterator got = textures.find(ID);
	if (got != textures.end())
	{
		return got->second;
	}

	cTextureShr texture = new cTexture(textureName);
	texture.setCustomDeallocator(this);

	textures[ID] = texture;
	return texture;
}

cFontShr cResources::getFont(const std::string& fontDataPath)
{
	std::string ID = textureID(fontDataPath);
	std::unordered_map<std::string, cFontShr>::iterator got = fonts.find(ID);
	if (got != fonts.end())
	{
		return got->second;
	}

	cFontShr font = new cFont(fontDataPath);
	font.setCustomDeallocator(this);

	fonts[ID] = font;
	return font;
}

void cResources::freeAll()
{
	bool err = false;
	for (unsigned i = 0; i < shaders.bucket_count(); ++i)
	{
		for (auto local_it = shaders.begin(i); local_it != shaders.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
			if (local_it->second.counter->getCount() != 1)
			{
				printf("Warning shader %s is not released after resource cleanup\n", local_it->first.c_str());
				err = true;
			}
		}
	}

	for (unsigned i = 0; i < textures.bucket_count(); ++i)
	{
		for (auto local_it = textures.begin(i); local_it != textures.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
			if (local_it->second.counter->getCount() != 1)
			{
				printf("Warning texture %s is not released after resource cleanup\n", local_it->first.c_str());
				err = true;
			}
		}
	}

	for (unsigned i = 0; i < fonts.bucket_count(); ++i)
	{
		for (auto local_it = fonts.begin(i); local_it != fonts.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
			if (local_it->second.counter->getCount() != 1)
			{
				printf("Warning font %s is not released after resource cleanup\n", local_it->first.c_str());
				err = true;
			}
		}
	}

	if (err)
	{
		assert(false);
	}
}

