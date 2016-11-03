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

std::string cResources::shaderID(const char* vertexShaderFile, const char* pixelShaderFile) const
{
	std::string ID = vertexShaderFile;
	ID += pixelShaderFile;
	return ID;
}

std::string cResources::textureID(const char* textureName) const
{
	std::string ID = textureName;
	return ID;
}

std::string cResources::fontId(const char* textureName) const
{
	std::string ID = textureName;
	return ID;
}

cResources::cResources()
{

}

cResources::~cResources()
{
	for (unsigned i = 0; i < shaders.bucket_count(); ++i)
	{
		for (auto local_it = shaders.begin(i); local_it != shaders.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
		}
	}

	for (unsigned i = 0; i < textures.bucket_count(); ++i)
	{
		for (auto local_it = textures.begin(i); local_it != textures.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
		}
	}

	for (unsigned i = 0; i < fonts.bucket_count(); ++i)
	{
		for (auto local_it = fonts.begin(i); local_it != fonts.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
		}
	}
}

cShaderShr cResources::getShader(const char* vertexShaderFile, const char* pixelShaderFile)
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

cTextureShr cResources::getTexture(const char* textureName)
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

cFontShr cResources::getFont(const char* fontDataPath)
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

