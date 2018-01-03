#pragma once

#include <vector>
#include "cRenderableWithShader.h"
#include "json.h"
#include "sol.h"
#include "DirentHelper.h"

class cParticle;

class cParticleRandomizer
{
	friend class cParticle;
	friend class cParticleTemplate;

	struct LinearRandomData
	{
		int attributeIndex;
		float min;
		float max;
	};

	struct VectorRandomData
	{
		int attributeIndex;
		Vec4 min;
		Vec4 max;
	};

	struct AngularRandomData
	{
		int attributeIndex;
		float minAngle;
		float maxAngle;
		float min;
		float max;
	};

	struct PositionRandom
	{
		bool isValid;
		bool isLinear;
		float minAngle;
		float maxAngle;
		float min;
		float max;
	};

	struct TextureRandomData
	{
		int attributeIndex;
		int textureIndexMin;
		int textureIndexMax;
	};

	struct UVRandomData
	{
		int attributeIndexStart;
		int attributeIndexSize;
		Vec2 min;
		Vec2 max;
	};

	PositionRandom positionRandom;
	cVector<LinearRandomData> linearRandoms;
	cVector<AngularRandomData> angularRandoms;
	cVector<TextureRandomData> textureRandoms;
	cVector<VectorRandomData> vectorRandoms;
	cVector<UVRandomData> uvRandoms;

public:
	cParticleRandomizer()
	{
		positionRandom.isValid = false;
	}

	void addLinear(int index, float min, float max)
	{
		LinearRandomData random;
		random.attributeIndex = index;
		random.min = min;
		random.max = max;
		for (auto& r : linearRandoms)
		{
			if (r.attributeIndex == index)
			{
				r = random;
				return;
			}
		}
		linearRandoms.push_back(random);
	}
	void addAngular(int index, float min, float max, float minAngle, float maxAngle)
	{
		AngularRandomData random;
		random.attributeIndex = index;
		random.min = min;
		random.max = max;
		random.minAngle = minAngle;
		random.maxAngle = maxAngle;
		for (auto& r : angularRandoms)
		{
			if (r.attributeIndex == index)
			{
				r = random;
				return;
			}
		}
		angularRandoms.push_back(random);
	}

	void addVector(int index, const Vec4& min, const Vec4& max)
	{
		VectorRandomData random;
		random.attributeIndex = index;
		random.min = min;
		random.max = max;
		for (auto& r : vectorRandoms)
		{
			if (r.attributeIndex == index)
			{
				r = random;
				return;
			}
		}
		vectorRandoms.push_back(random);
	}

	void addUVRandom(int indexStart, int indexSize, const Vec2& min, const Vec2& max)
	{
		UVRandomData random;
		random.attributeIndexStart = indexStart;
		random.attributeIndexSize = indexSize;
		random.min = min;
		random.max = max;
		for (auto& r : uvRandoms)
		{
			if (r.attributeIndexStart == indexStart)
			{
				r = random;
				return;
			}
		}
		uvRandoms.push_back(random);
	}

	void setPosShiftLinear(float min, float max)
	{
		positionRandom.isValid = true;
		positionRandom.isLinear = true;
		positionRandom.min = min;
		positionRandom.max = max;
	}

	void setPosShiftAngular(float min, float max, float minAngle, float maxAngle)
	{
		positionRandom.isValid = true;
		positionRandom.isLinear = false;
		positionRandom.min = min;
		positionRandom.max = max;
		positionRandom.minAngle = minAngle;
		positionRandom.maxAngle = maxAngle;
	}
};

class cParticleTemplate : public cUniformDataWithShader
{
	friend class cParticle;
	cParticleRandomizer randomizer;
	float maxLifeTime;
	std::string scriptName;
	sol::table scriptTable;

	int attributeSize;

	bool isStrip;
	bool isStripLaser;

	struct Attribute
	{
		int index;
		std::string name;
		int type;
		int size;
		int begin;
	};

	cVector<Attribute> attributes;
	std::unordered_map<std::string, int> attributesMap;
	cVector<cTextureShr> textures;
	std::unordered_map<std::string, int> texturesMap;
	cVector<GLuint> emptyBuffers;

	int posIndex;
	int timeIndex;
	int uvIndex;

	struct Uniform
	{
		int index;
		std::string name;
		int type;
	};
	cVector<Uniform> uniforms;

	int uCurrentTime;
	bool needsLuaRandoms;
	void randomizeAttributes(cParticleRandomizer& randomizer, cParticle* particle, Vec2& pos, char *buff, cVector<int>& setAttributes) const;

public:
	cParticleTemplate(nlohmann::json& j, const DirentHelper::File& file);

	~cParticleTemplate()
	{
		shader = nullptr;
		for (auto& t : textures)
		{
			t = nullptr;
		}
		textures.clear();
		for (auto& t : emptyBuffers)
		{
			glDeleteBuffers(1, &t);
		}
	}

	const std::string& getName() const
	{
		return scriptName;
	}

	int getAttributeIndex(const std::string& name)
	{
		return attributesMap[name];
	}
	bool needsLuaCall() const;
};


class cParticle : public cRenderableWithShader
{
	float time;
	cParticleTemplate *particleTemplate;
	char *buff;

	struct QuadBufferData
	{
		GLuint quadBuffer;
		int count;
		float timeToDie;
	};

	int maxBufferSize;
	cVector<QuadBufferData> quadBuffers;

	bool nextIsStripBegining;
	cVector<cTextureShr> textures;
	cUniformDataWithShader uniformData;

	cParticleRandomizer randomizer;
public:
	sol::table args; // todo move to private

	cParticle(cGame* game, cParticleTemplate *particleTemplate, const sol::table& args);

	void setTexture(const std::string& path);

	void addTexture(const std::string& path);

	~cParticle()
	{
		textures.clear();
		SAFE_DELETE(buff);
		clear();
	}

	void addParticleInternal(const Vec2& pos, sol::table* params, cParticleRandomizer* randomizer = nullptr, bool bReplaceLast = false);
	void addParticle(const Vec2& pos, sol::table& params)
	{
		addParticleInternal(pos, &params, nullptr);
	}
	void addParticleWithoutArgs(const Vec2& pos, cParticleRandomizer* randomizer = nullptr)
	{
		addParticleInternal(pos, nullptr, randomizer);
	}

	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop);

	bool hasParticle()
	{
		return quadBuffers.size() > 0;
	}

	void clear()
	{
		for (auto& bufferData : quadBuffers)
		{
			if (particleTemplate->emptyBuffers.size() < maxBufferSize * 2)
			{
				particleTemplate->emptyBuffers.push_back(bufferData.quadBuffer);
			}
			else
			{
				glDeleteBuffers(1, &bufferData.quadBuffer);
			}
		}
		quadBuffers.clear();
	}

	cParticleTemplate* getParticleTemplate() const
	{
		return particleTemplate;
	}

	void addLinearRandomizerWithName(const std::string& name, float min, float max)
	{
		randomizer.addLinear(particleTemplate->attributesMap[name], min, max);
	}

	void addAngularRandomizerWithName(const std::string& name, float min, float max, float minAngle, float maxAngle)
	{
		randomizer.addAngular(particleTemplate->attributesMap[name], min, max, minAngle, maxAngle);
	}

	void addVectorRandomizerWithName(const std::string& name, const Vec4& min, const Vec4& max)
	{
		randomizer.addVector(particleTemplate->attributesMap[name], min, max);
	}

	void addUVRandomizerWithName(const std::string& start, const std::string& size, const Vec2& min, const Vec2& max)
	{
		randomizer.addUVRandom(particleTemplate->attributesMap[start], particleTemplate->attributesMap[size], min, max);
	}

	void addLinearRandomizer(int index, float min, float max)
	{
		randomizer.addLinear(index, min, max);
	}

	void addAngularRandomizer(int index, float min, float max, float minAngle, float maxAngle)
	{
		randomizer.addAngular(index, min, max, minAngle, maxAngle);
	}

	void addVectorRandomizer(int index, const Vec4& min, const Vec4& max)
	{
		randomizer.addVector(index, min, max);
	}

	void addUVRandomRandomizer(int indexStart, int indexSize, const Vec2& min, const Vec2& max)
	{
		randomizer.addUVRandom(indexStart, indexSize, min, max);
	}

	void setPosShiftLinear(float min, float max)
	{
		randomizer.setPosShiftLinear(min, max);
	}

	void setPosShiftAngular(float min, float max, float minAngle, float maxAngle)
	{
		randomizer.setPosShiftAngular(min, max, minAngle, maxAngle);
	}

	float getStartTime() const
	{
		return time;
	}
};