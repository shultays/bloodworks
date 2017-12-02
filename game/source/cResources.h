#pragma once

#include "cSharedPtr.h"
#include "cVector.h"
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

	std::string textureID(const std::string& textureName, bool repeat) const;

	std::string fontID(const std::string& textureName) const;

	std::string soundSampleID(const std::string& sampleName) const;

	template<class T, int bucket, int lifeTime>
	class Graveyard
	{
	public:
		struct Data
		{
			std::string ID;
			cSharedPtr<T> object;
			float timeToDie;
		};
		Data objects[bucket];
		int cur;
		int count;
		Graveyard()
		{
			for (int i = 0; i < bucket; i++)
			{
				objects[i].object = nullptr;
			}
			cur = 0;
			count = 0;
		}

		void add(const std::string& ID, const cSharedPtr<T>& object, float t)
		{
			if (count == 0)
			{
				cur = 0;
			}

			if (count == bucket)
			{
				objects[cur].object = nullptr;
			}
			else
			{
				count++;
			}
			objects[cur].ID = ID;
			objects[cur].object = object;
			objects[cur].timeToDie = t + lifeTime;
			cur++;
			if (cur == bucket)
			{
				cur = 0;
			}
		}

		cSharedPtr<T> get(const std::string& ID)
		{
			int t = count;
			int i = cur;
			while (t-- > 0)
			{
				i--;
				if (i == -1)
				{
					i = bucket - 1;
				}
				if (objects[i].ID == ID)
				{
					objects[i].ID = "";
					cSharedPtr<T> r = objects[i].object;
					objects[i].object = nullptr;
					return r;
				}
			}
			return nullptr;
		}

		void tick(float t)
		{
			int i = cur - count;
			if (i < 0)
			{
				i += bucket;
			}

			while (count > 0)
			{
				if (objects[i].object == nullptr || objects[i].timeToDie < t)
				{
					objects[i].object = nullptr;
					objects[i].ID = "";
					count--;
				}
				else
				{
					break;
				}

				i++;
				if (i == bucket)
				{
					i = 0;
				}
			}
		}

		void clear()
		{
			while (count-- > 0)
			{
				cur--;
				if (cur == -1)
				{
					cur = bucket - 1;
				}
				objects[cur].object = nullptr;
			}
		}
	};

	Graveyard<cTexture, 256, 30> textureGraveyard;
	Graveyard<cShader, 32, 30> shaderGraveyard;
	Graveyard<cSoundSample, 128, 30> soundGraveyard;
	Graveyard<cFont, 16, 30> fontGraveyard;
public:
	cResources();

	~cResources();

	cShaderShr getShader(const std::string& file);

	cShaderShr getShader(const std::string& vertexShaderFile, const std::string& pixelShaderFile);

	cTextureShr getTexture(const std::string& textureName, bool repeat = false);

	cFontShr getFont(const std::string& fontDataPath);

	cSoundSampleShr getSoundSample(const std::string& soundSamplePath);

	void freeAll();

	void tick();
};
