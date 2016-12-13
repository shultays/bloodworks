#pragma once

#pragma once

#include "cSharedPtr.h"
#include <unordered_map>

class cShader;
class cTexture;
class cFont;

typedef cSharedPtr<cShader> cShaderShr;
typedef cSharedPtr<cTexture> cTextureShr;
typedef cSharedPtr<cFont> cFontShr;

class cResources : public cCustomDeallocator<cShader>, public cCustomDeallocator<cTexture>, public cCustomDeallocator<cFont>
{
	std::unordered_map<std::string, cShaderShr> shaders;
	std::unordered_map<std::string, cTextureShr> textures;
	std::unordered_map<std::string, cFontShr> fonts;

	void objectFreed(cShader* object) override;

	void objectFreed(cTexture* object) override;

	void objectFreed(cFont* object) override;

	std::string shaderID(const std::string& vertexShaderFile, const std::string& pixelShaderFile) const;

	std::string textureID(const std::string& textureName) const;

	std::string fontId(const std::string& textureName) const;

public:
	cResources();

	~cResources();

	cShaderShr getShader(const std::string& vertexShaderFile, const std::string& pixelShaderFile);

	cTextureShr getTexture(const std::string& textureName);

	cFontShr getFont(const std::string& fontDataPath);

	void freeAll();

	static void deleteObject(cShader* object);
	static void deleteObject(cTexture* object);
	static void deleteObject(cFont* object);
};

