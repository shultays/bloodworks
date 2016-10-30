#pragma once

#pragma once

#include "cShader.h"
#include "cTexture.h"
#include "cTools.h"
#include "cSharedPtr.h"

typedef cSharedPtr<cShader> cShaderShr;
typedef cSharedPtr<cTexture> cTextureShr;

class cResources : public cCustomDeallocator<cShader>, public cCustomDeallocator<cTexture> 
{
	std::unordered_map<std::string, cShaderShr> shaders;
	std::unordered_map<std::string, cTextureShr> textures;

	void objectFreed(cShader* object) override 
	{
		std::string ID = shaderID(object->getVertexShaderFileName(), object->getPixelShaderFileName());
		std::unordered_map<std::string, cShaderShr>::iterator got = shaders.find(ID);
		assert(got != shaders.end());
		got->second.setCustomDeallocator(nullptr);
		shaders.erase(got);
	}

	void objectFreed(cTexture* object) override 
	{
		std::string ID = textureID(object->getName());
		std::unordered_map<std::string, cTextureShr>::iterator got = textures.find(ID);
		assert(got != textures.end());
		got->second.setCustomDeallocator(nullptr);
		textures.erase(got);
	}

	std::string shaderID(const char* vertexShaderFile, const char* pixelShaderFile) const 
	{
		std::string ID = vertexShaderFile;
		ID += pixelShaderFile;
		return ID;
	}

	std::string textureID(const char* textureName) const 
	{
		std::string ID = textureName;
		return ID;
	}
public:

	cResources() 
	{
	}

	~cResources() 
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
	}

	cShaderShr getShader(const char* vertexShaderFile, const char* pixelShaderFile) 
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

	cTextureShr getTexture(const char* textureName) 
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
};

