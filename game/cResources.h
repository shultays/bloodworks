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

	std::string shaderID(const char* vertexShaderFile, const char* pixelShaderFile) const;

	std::string textureID(const char* textureName) const;

	std::string fontId(const char* textureName) const;
public:
	cResources();

	~cResources();

	cShaderShr getShader(const char* vertexShaderFile, const char* pixelShaderFile);

	cTextureShr getTexture(const char* textureName);

	cFontShr getFont(const char* fontDataPath);
};

