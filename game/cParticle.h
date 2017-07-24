#pragma once

#include <vector>
#include "cRenderableWithShader.h"
#include "json.h"
#include "sol.h"
#include "DirentHelper.h"

#define MAX_QUAD 256

class cParticleTemplate
{
	friend class cParticle;

	float maxLifeTime;
	cShaderShr shader;
	std::string scriptName;
	sol::table scriptTable;

	int attributeSize;
	int uvIndex;

	bool isStrip;

	struct Attribute
	{
		int index;
		std::string name;
		int type;
		int size;
		int begin;
	};

	std::vector<Attribute> attributes;
	std::vector<cTextureShr> textures;
	std::vector<GLuint> emptyBuffers;

	struct Uniform
	{
		int index;
		std::string name;
		int type;
	};
	std::vector<Uniform> uniforms;

	int uCurrentTime;
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
	std::vector<QuadBufferData> quadBuffers;

	bool nextIsStripBegining;
	std::vector<cTextureShr> textures;

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

	void addParticle(const Vec2& pos, sol::table& params);

	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop);

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
};