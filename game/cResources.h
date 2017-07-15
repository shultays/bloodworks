#pragma once

#include "cSharedPtr.h"
#include <unordered_map>

class cShader;
class cTexture;
class cFont;
class cSoundSample;

typedef cSharedPtr<cShader> cShaderShr;
typedef cSharedPtr<cTexture> cTextureShr;
typedef cSharedPtr<cFont> cFontShr;
typedef cSharedPtr<cSoundSample> cSoundSampleShr;

class cResources : public cCustomDeallocator<cShader>, public cCustomDeallocator<cTexture>, public cCustomDeallocator<cFont>, public cCustomDeallocator<cSoundSample>
{
	std::unordered_map<std::string, cShaderShr> shaders;
	std::unordered_map<std::string, cTextureShr> textures;
	std::unordered_map<std::string, cFontShr> fonts;
	std::unordered_map<std::string, cSoundSampleShr> soundSamples;

	void objectFreed(cShader* object) override;

	void objectFreed(cTexture* object) override;

	void objectFreed(cFont* object) override;

	void objectFreed(cSoundSample* object) override;

	std::string shaderID(const std::string& vertexShaderFile, const std::string& pixelShaderFile) const;

	std::string textureID(const std::string& textureName) const;

	std::string fontID(const std::string& textureName) const;

	std::string soundSampleID(const std::string& sampleName) const;

public:
	cResources();

	~cResources();

	cShaderShr getShader(const std::string& file);

	cShaderShr getShader(const std::string& vertexShaderFile, const std::string& pixelShaderFile);

	cTextureShr getTexture(const std::string& textureName);

	cFontShr getFont(const std::string& fontDataPath);

	cSoundSampleShr getSoundSample(const std::string& soundSamplePath);

	void freeAll();

	static void deleteObject(cShader* object);
	static void deleteObject(cTexture* object);
	static void deleteObject(cFont* object);
	static void deleteObject(cSoundSample* object);
};

