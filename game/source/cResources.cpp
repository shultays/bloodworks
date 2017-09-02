#include "cResources.h"
#include "cFont.h"
#include "cTexture.h"
#include "cShader.h"
#include "cSoundManager.h"
#include "cSound.h"
#include "cSharedPtr.h"

int nextSharedPtrUniqueId = 0;

int totalResource = 0;

void cResources::objectFreed(cFont* object)
{
	std::string ID = fontID(object->getFontDataPath());
	std::unordered_map<std::string, cFontShr>::iterator got = fonts.find(ID);
	assert(got != fonts.end());
	got->second.setCustomDeallocator(nullptr);
	fontGraveyard.add(ID, got->second, timer.getRenderTime());
	fonts.erase(got);
}

void cResources::objectFreed(cTexture* object)
{
	std::string ID = textureID(object->getName(), object->isRepeat());
	std::unordered_map<std::string, cTextureShr>::iterator got = textures.find(ID);
	assert(got != textures.end());
	got->second.setCustomDeallocator(nullptr);
	textureGraveyard.add(ID, got->second, timer.getRenderTime());
	textures.erase(got);
}

void cResources::objectFreed(cShader* object)
{
	std::string ID = shaderID(object->getVertexShaderFileName(), object->getPixelShaderFileName());
	std::unordered_map<std::string, cShaderShr>::iterator got = shaders.find(ID);
	assert(got != shaders.end());
	got->second.setCustomDeallocator(nullptr);
	shaderGraveyard.add(ID, got->second, timer.getRenderTime());
	shaders.erase(got);
}

void cResources::objectFreed(cSoundSample* object)
{
	std::string ID = soundSampleID(object->getName());
	std::unordered_map<std::string, cSoundSampleShr>::iterator got = soundSamples.find(ID);
	assert(got != soundSamples.end());
	got->second.setCustomDeallocator(nullptr);
	soundGraveyard.add(ID, got->second, timer.getRenderTime());
	soundSamples.erase(got);
}

std::string cResources::shaderID(const std::string& vertexShaderFile, const std::string& pixelShaderFile) const
{
	std::string ID = vertexShaderFile;
	ID += pixelShaderFile;
	return ID;
}

std::string cResources::textureID(const std::string& textureName, bool repeat) const
{
	std::string ID = textureName;
	if (repeat)
	{
		ID += "repeat";
	}
	return ID;
}

std::string cResources::fontID(const std::string& textureName) const
{
	std::string ID = textureName;
	return ID;
}

std::string cResources::soundSampleID(const std::string& sampleName) const
{
	std::string ID = sampleName;
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

void cResources::deleteObject(cSoundSample* object)
{
	delete object;
}

void cResources::tick()
{
	float time = timer.getRenderTime();
	textureGraveyard.tick(time);
	shaderGraveyard.tick(time);
	soundGraveyard.tick(time);
	fontGraveyard.tick(time);
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
	std::string vpath = vertexShaderFile;
	fixFilePath(vpath);
	std::string ppath = pixelShaderFile;
	fixFilePath(ppath);

	std::string ID = shaderID(vpath, ppath);
	std::unordered_map<std::string, cShaderShr>::iterator got = shaders.find(ID);
	if (got != shaders.end())
	{
		return got->second;
	}

	cShaderShr shader = shaderGraveyard.get(ID);
	if (shader == nullptr)
	{
		shader = new cShader();
		shader->loadFromFile(vpath, ppath);
	}
	shader.setCustomDeallocator(this);

	shaders[ID] = shader;
	return shader;
}

cTextureShr cResources::getTexture(const std::string& textureName, bool repeat)
{
	std::string path = textureName;
	fixFilePath(path);
	std::string ID = textureID(path, repeat);
	std::unordered_map<std::string, cTextureShr>::iterator got = textures.find(ID);
	if (got != textures.end())
	{
		return got->second;
	}

	cTextureShr texture = textureGraveyard.get(ID);
	if (texture == nullptr)
	{
		texture = new cTexture(path, repeat);
	}
	texture.setCustomDeallocator(this);

	textures[ID] = texture;
	return texture;
}

cSoundSampleShr cResources::getSoundSample(const std::string& soundSamplePathNotFixed)
{
	std::string soundSamplePath = soundSamplePathNotFixed;
	fixFilePath(soundSamplePath);
	std::string ID = soundSampleID(soundSamplePath);
	std::unordered_map<std::string, cSoundSampleShr>::iterator got = soundSamples.find(ID);

	if (got != soundSamples.end())
	{
		return got->second;
	}

	cSoundSampleShr sample = soundGraveyard.get(ID);
	if (sample == nullptr)
	{
		sample = new cSoundSample(coral.getSoundManager());
		sample->loadSample(soundSamplePath);
	}
	
	sample.setCustomDeallocator(this);

	soundSamples[ID] = sample;
	return sample;
}

cFontShr cResources::getFont(const std::string& fontDataPath)
{
	std::string path = fontDataPath;
	fixFilePath(path);

	std::string ID = textureID(path, false);
	std::unordered_map<std::string, cFontShr>::iterator got = fonts.find(ID);
	if (got != fonts.end())
	{
		return got->second;
	}

	cFontShr font = fontGraveyard.get(ID);

	if (font == nullptr)
	{
		font = new cFont(path);
	}

	font.setCustomDeallocator(this);

	fonts[ID] = font;
	return font;
}

void cResources::freeAll()
{
	fontGraveyard.clear();
	soundGraveyard.clear();
	shaderGraveyard.clear();
	textureGraveyard.clear();

	bool err = false;
	for (unsigned i = 0; i < shaders.bucket_count(); ++i)
	{
		for (auto local_it = shaders.begin(i); local_it != shaders.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
			if (local_it->second.counter->getCount() != 1)
			{
				out << "Warning shader " << local_it->first << "is not released after resource cleanup\n";
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
				out << "Warning texture " << local_it->first << "is not released after resource cleanup\n";
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
				out << "Warning font " << local_it->first << "is not released after resource cleanup\n";
				err = true;
			}
		}
	}

	for (unsigned i = 0; i < soundSamples.bucket_count(); ++i)
	{
		for (auto local_it = soundSamples.begin(i); local_it != soundSamples.end(i); ++local_it)
		{
			local_it->second.setCustomDeallocator(nullptr);
			if (local_it->second.counter->getCount() != 1)
			{
				out << "Warning sound sample " << local_it->first << "is not released after resource cleanup\n";
				err = true;
			}
		}
	}

	if (totalResource != 0)
	{
		out << "Total resource " << totalResource << "\n";
		err = true;
	}
	if (err)
	{
		assert(false);
	}
}
