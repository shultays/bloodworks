#include "cParticle.h"
#include "cGlobals.h"
#include "cShader.h"
#include "cGame.h"
#include "cTexture.h"

cParticleTemplate::cParticleTemplate(nlohmann::json& j)
{
	shader = resources.getShader(j["vertexShader"].get<std::string>(), j["pixelShader"].get<std::string>());
	scriptName = j["scriptName"].get<std::string>();
	scriptTable = lua[scriptName] = lua.create_table();
	std::string scriptPath = j["scriptFile"].get<std::string>();
	lua.script_file(scriptPath);

	maxLifeTime = j["maxLifeTime"].get<float>();
	attributeSize = 0;

	auto addAtribute = [&](const std::string& attributeName, const std::string& attributeType)
	{
		if (attributeName == "uv")
		{
			uvIndex = (int)attributes.size();
		}
		Attribute attribute;
		attribute.name = attributeName;

		if (attributeType == "float")
		{
			attribute.type = TypeFloat;
		}
		else if (attributeType == "vec2")
		{
			attribute.type = TypeVec2;
		}
		else if (attributeType == "vec3")
		{
			attribute.type = TypeVec3;
		}
		else if (attributeType == "vec4")
		{
			attribute.type = TypeVec4;
		}
		else
		{
			assert(!"Unknown attribute type");
		}

		const cShader::Attribute& a = shader->addAttribute(attributeName, attribute.type);
		attribute.index = a.index;
		attribute.begin = attributeSize;
		attribute.size = a.getCount() * 4;
		attributeSize += attribute.size;

		attributes.push_back(attribute);
	};
	addAtribute("pos", "vec2");
	addAtribute("uv", "vec2");
	addAtribute("time", "float");

	uCurrentTime = shader->addUniform("uCurrentTime", TypeFloat).index;

	lua.set_function("addAttribute", addAtribute);


	auto addUniform = [&](const std::string& uniformName, const std::string& uniformType)
	{
		Uniform uniform;

		if (uniformType == "float")
		{
			uniform.type = TypeFloat;
		}
		else if (uniformType == "vec2")
		{
			uniform.type = TypeVec2;
		}
		else if (uniformType == "vec3")
		{
			uniform.type = TypeVec3;
		}
		else if (uniformType == "vec4")
		{
			uniform.type = TypeVec4;
		}
		else
		{
			assert(!"Unknown uniform type");
		}
		uniform.name = uniformName;
		uniform.index = shader->addUniform(uniformName, uniform.type).index;
		uniforms.push_back(uniform);
	};

	lua.set_function("addUniform", addUniform);

	if (j.count("isStrip"))
	{
		isStrip = j["isStrip"].get<bool>();
	}
	else
	{
		isStrip = false;
	}

	if (j["textures"].is_array())
	{
		for (auto& t : j["textures"])
		{
			textures.push_back(resources.getTexture(t.get<std::string>()));
		}
	}
	else
	{
		textures.push_back(resources.getTexture(j["textures"].get<std::string>()));
	}
	scriptTable["initSystem"]();
}

cParticle::cParticle(cGame* game, cParticleTemplate *particleTemplate, const sol::table& args) : cRenderableWithShader(game, particleTemplate->shader)
{
	this->particleTemplate = particleTemplate;
	time = timer.getTime();

	buff = new char[particleTemplate->attributeSize * 4];
	this->args = args;
	maxBufferSize = 0;
	nextIsStripBegining = true;

	if (particleTemplate->scriptTable["setDefaultArgs"])
	{
		particleTemplate->scriptTable["setDefaultArgs"](args);
	}
	textures = particleTemplate->textures;
}

void cParticle::setTexture(const std::string& path)
{
	cTextureShr texture = resources.getTexture(path);
	if (textures.size() == 0)
	{
		textures.push_back(texture);
	}
	else
	{
		textures[0] = texture;
	}
}

void cParticle::addTexture(const std::string& path)
{
	cTextureShr texture = resources.getTexture(path);
	textures.push_back(texture);
}

void cParticle::addParticle(const Vec2& pos, sol::table& params)
{
	if (quadBuffers.size() == 0 || quadBuffers[quadBuffers.size() - 1].count == MAX_QUAD * 4)
	{
		QuadBufferData bufferData;
		bufferData.count = 0;

		if (particleTemplate->emptyBuffers.size() == 0)
		{
			glGenBuffers(1, &bufferData.quadBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, bufferData.quadBuffer);
			glBufferData(GL_ARRAY_BUFFER, particleTemplate->attributeSize * 4 * MAX_QUAD, NULL, GL_DYNAMIC_DRAW);
		}
		else
		{
			bufferData.quadBuffer = particleTemplate->emptyBuffers[(int)particleTemplate->emptyBuffers.size() - 1];
			particleTemplate->emptyBuffers.resize(particleTemplate->emptyBuffers.size() - 1);
		}


		quadBuffers.push_back(bufferData);
	}

	QuadBufferData &bufferData = quadBuffers[quadBuffers.size() - 1];
	params["particleBeginTime"] = time;
	particleTemplate->scriptTable["addParticle"](params, pos, args);

	int particleNumToAdd = particleTemplate->isStrip ? 2 : 4;

	memset(buff, 0, particleTemplate->attributeSize * particleNumToAdd);

	int vertexSize = particleTemplate->attributeSize;

	for (auto& uniform : particleTemplate->uniforms)
	{
		if (params[uniform.name])
		{
			switch (uniform.type)
			{
			case TypeFloat:
				setUniform(uniform.index, params[uniform.name].get<float>());
				break;
			case TypeVec2:
				setUniform(uniform.index, params[uniform.name].get<Vec2>());
				break;
			case TypeVec3:
				setUniform(uniform.index, params[uniform.name].get<Vec3>());
				break;
			case TypeVec4:
				setUniform(uniform.index, params[uniform.name].get<Vec4>());
				break;
			}
		}
	}

	for (auto& attribute : particleTemplate->attributes)
	{
		if (params[attribute.name])
		{
			switch (attribute.type)
			{
			case TypeFloat:
				*(float*)(buff + attribute.begin) = params[attribute.name].get<float>();
				break;
			case TypeVec2:
				*(Vec2*)(buff + attribute.begin) = params[attribute.name].get<Vec2>();
				break;
			case TypeVec3:
				*(Vec3*)(buff + attribute.begin) = params[attribute.name].get<Vec3>();
				break;
			case TypeVec4:
				*(Vec4*)(buff + attribute.begin) = params[attribute.name].get<Vec4>();
				break;
			}
		}
		else if (attribute.name == "time")
		{
			*(float*)(buff + attribute.begin) = timer.getTime() - time;
		}
		else if (attribute.name == "uv")
		{
		}
		else
		{
			assert(!"attribute is not set");
		}
	}

	if (particleNumToAdd == 4)
	{
		memcpy(buff + vertexSize * 1, buff, vertexSize);
		memcpy(buff + vertexSize * 2, buff, vertexSize);
		memcpy(buff + vertexSize * 3, buff, vertexSize);


		*(Vec2*)(buff + vertexSize * 0 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 0.0f);
		*(Vec2*)(buff + vertexSize * 1 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 1.0f);
		*(Vec2*)(buff + vertexSize * 2 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 1.0f);
		*(Vec2*)(buff + vertexSize * 3 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 0.0f);
	}
	else
	{
		memcpy(buff + vertexSize * 1, buff, vertexSize);

		if (nextIsStripBegining)
		{
			*(Vec2*)(buff + vertexSize * 0 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 0.0f);
			*(Vec2*)(buff + vertexSize * 1 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 1.0f);
		}
		else
		{
			*(Vec2*)(buff + vertexSize * 0 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 0.0f);
			*(Vec2*)(buff + vertexSize * 1 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 1.0f);
		}
		nextIsStripBegining = !nextIsStripBegining;
	}

	glBindBuffer(GL_ARRAY_BUFFER, bufferData.quadBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, vertexSize * bufferData.count, vertexSize * particleNumToAdd, buff);

	bufferData.count += particleNumToAdd;
	bufferData.timeToDie = timer.getTime() + particleTemplate->maxLifeTime;
}

void cParticle::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	if (quadBuffers.size() > 0)
	{
		for (int i = 0; i < quadBuffers.size(); i++)
		{
			if (quadBuffers[i].timeToDie < timer.getTime())
			{
				particleTemplate->emptyBuffers.push_back(quadBuffers[i].quadBuffer);
				for (int j = i; j < quadBuffers.size() - 1; j++)
				{
					quadBuffers[j] = quadBuffers[j + 1];
				}
				quadBuffers.resize(quadBuffers.size() - 1);
				i--;
			}
		}

		game->lastShader = particleTemplate->shader;
		particleTemplate->shader->begin();
		particleTemplate->shader->setUniform(particleTemplate->uCurrentTime, timer.getTime() - time);

		cRenderableWithShader::render(isIdentity, mat, crop);
		glActiveTexture(GL_TEXTURE0);

		for (int i = (int)textures.size() - 1; i >= 0; i--)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i]->bindTexture();
			particleTemplate->shader->setTexture(i, i);
		}

		particleTemplate->shader->setViewMatrix(game->getViewMatrix(alignment));

		glEnableVertexAttribArray(0);
		for (auto& bufferData : quadBuffers)
		{
			if (bufferData.count >= 4)
			{
				glBindBuffer(GL_ARRAY_BUFFER, bufferData.quadBuffer);

				for (auto& attribute : particleTemplate->attributes)
				{
					particleTemplate->shader->bindAttribute(attribute.index, particleTemplate->attributeSize, attribute.begin);
				}
				if (particleTemplate->isStrip)
				{
					glDrawArrays(GL_QUAD_STRIP, 0, bufferData.count);
				}
				else
				{
					glDrawArrays(GL_QUADS, 0, bufferData.count);
				}
			}
		}

		glDisableVertexAttribArray(0);

		glDisable(GL_TEXTURE_2D);
		game->lastShader = nullptr;
	}
}
