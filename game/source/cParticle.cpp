#include "cParticle.h"
#include "cGlobals.h"
#include "cShader.h"
#include "cGame.h"
#include "cTexture.h"
#include "cTools.h"
#include "cTools.h"
#include "cTimeProfiler.h"

#define MAX_QUAD 256

bool disableParticle;

cParticleTemplate::cParticleTemplate(nlohmann::json& j, const DirentHelper::File& file)
{
	shader = resources.getShader(file.folder + j["vertexShader"].get<std::string>(), file.folder + j["pixelShader"].get<std::string>());
	scriptName = j["scriptName"].get<std::string>();
	scriptTable = lua[scriptName] = lua.create_table();
	std::string folder = file.folder;
	fixFolderPath(folder);
	scriptTable["basePath"] = folder;
	std::string scriptPath = file.folder + j["scriptFile"].get<std::string>();
	lua.script_file(scriptPath);

	maxLifeTime = j["maxLifeTime"].get<float>();
	attributeSize = 0;

	if (j.count("isStrip"))
	{
		isStrip = j["isStrip"].get<bool>();
	}
	else
	{
		isStrip = false;
	}
	

	isStripLaser = false;

	if (j.count("isStripLaser"))
	{
		isStripLaser = j["isStripLaser"].get<bool>();
	}

	if (isStripLaser)
	{
		isStrip = true;
	}

	auto addAtribute = [&](const std::string& attributeName, const std::string& attributeType)
	{
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
		attributesMap[attributeName] = attributes.size() - 1;
	};
	addAtribute("pos", "vec2");
	addAtribute("time", "float");
	addAtribute("uv", "vec2");

	posIndex = attributesMap["pos"];
	timeIndex = attributesMap["time"];
	uvIndex = attributesMap["uv"];

	if (isStripLaser) 
	{
		addAtribute("speed", "vec2");
		addAtribute("prevpos", "vec2");
		addAtribute("prevspeed", "vec2");
		addAtribute("prevtime", "float");
	}

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

	if (j.count("textures"))
	{
		if (j["textures"].is_array())
		{
			for (auto& t : j["textures"])
			{
				std::string name = t.get<std::string>();
				textures.push_back(resources.getTexture(file.folder + name, true));
				texturesMap[name] = textures.size() - 1;
			}
		}
		else
		{
			std::string name = j["textures"].get<std::string>();
			textures.push_back(resources.getTexture(file.folder + name, true));
			texturesMap[name] = 0;
		}
	}

	needsLuaRandoms = true;
	if (j.count("needsLuaRandoms"))
	{
		needsLuaRandoms = j["needsLuaRandoms"].get<bool>();
	}
	scriptTable["initSystem"]();

	if (j.count("uniformRandoms"))
	{
		for (auto& r : j["uniformRandoms"])
		{
			std::string t = r["type"].get<std::string>();
			if (t == "linear")
			{
				cParticleRandomizer::LinearRandomData random;
				random.attributeIndex = attributesMap[r["name"].get<std::string>()];
				if (r.count("min"))
				{
					random.min = r["min"].get<float>();
					random.max = r["max"].get<float>();
				}
				else
				{
					random.min = 0.0f;
					random.max = 1.0f;
				}
				randomizer.linearRandoms.push_back(random);
			}
			else if (t == "angular")
			{
				cParticleRandomizer::AngularRandomData random;
				random.attributeIndex = attributesMap[r["name"].get<std::string>()];
				if (r.count("min"))
				{
					random.min = r["min"].get<float>();
					random.max = r["max"].get<float>();
				}
				else
				{
					random.min = 0.0f;
					random.max = 1.0f;
				}
				if (r.count("minAngle"))
				{
					random.minAngle = r["minAngle"].get<float>() * pi_2;
					random.maxAngle = r["maxAngle"].get<float>() * pi_2;
				}
				else
				{
					random.minAngle = 0.0f;
					random.maxAngle = pi_2;
				}
				randomizer.angularRandoms.push_back(random);
			}
			else if (t == "texture")
			{
				cParticleRandomizer::TextureRandomData random;
				random.attributeIndex = attributesMap[r["name"].get<std::string>()];
				if (r.count("path"))
				{
					random.textureIndexMin = random.textureIndexMax = texturesMap[r["path"].get<std::string>()];
				}
				else
				{
					random.textureIndexMin = r["min"].get<int>();
					random.textureIndexMax = r["max"].get<int>();
				}
				randomizer.textureRandoms.push_back(random);
			}
			else if (t == "vector")
			{
				cParticleRandomizer::VectorRandomData random;
				random.attributeIndex = attributesMap[r["name"].get<std::string>()];
				auto& minColor = r["min"];
				auto& maxColor = r["max"];
				for (int i = 0; i < 4; i++)
				{
					if (i < minColor.size())
					{
						random.min[i] = minColor.at(i).get<float>();
						random.max[i] = maxColor.at(i).get<float>();
					}
					else
					{
						random.min[i] = random.max[i] = 1.0f;
					}
				}
				randomizer.vectorRandoms.push_back(random);
			}
			else if (t == "uvRandom")
			{
				cParticleRandomizer::UVRandomData random;
				random.attributeIndexStart = attributesMap[r["start"].get<std::string>()];
				random.attributeIndexSize = attributesMap[r["size"].get<std::string>()];
				if (r["min"].is_array())
				{
					random.min[0] = r["min"].at(0).get<float>();
					random.min[1] = r["min"].at(1).get<float>();
					random.max[0] = r["max"].at(0).get<float>();
					random.max[1] = r["max"].at(1).get<float>();
				}
				else
				{
					random.min[0] = random.min[1] = r["min"].get<float>();
					random.max[0] = random.max[1] = r["max"].get<float>();
				}
				randomizer.uvRandoms.push_back(random);
			}
			else if (t == "positionShift")
			{
				randomizer.positionRandom.isValid = true;
				if (r.count("isLinear"))
				{
					randomizer.positionRandom.isLinear = r["isLinear"].get<bool>();
				}
				else
				{
					randomizer.positionRandom.isLinear = false;
				}
				if (r.count("min"))
				{
					randomizer.positionRandom.min = r["min"].get<float>();
					randomizer.positionRandom.max = r["max"].get<float>();
				}
				else
				{
					randomizer.positionRandom.min = 0.0f;
					randomizer.positionRandom.max = 1.0f;
				}
				if (r.count("minAngle"))
				{
					randomizer.positionRandom.minAngle = r["minAngle"].get<float>() * pi_2;
					randomizer.positionRandom.maxAngle = r["maxAngle"].get<float>() * pi_2;
				}
				else
				{
					randomizer.positionRandom.minAngle = 0.0f;
					randomizer.positionRandom.maxAngle = pi_2;
				}
			}
			else
			{
				assert(false);
			}
		}
	}
}

bool cParticleTemplate::needsLuaCall() const
{
	return needsLuaRandoms;
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
	cTextureShr texture = resources.getTexture(path, true);
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
	cTextureShr texture = resources.getTexture(path, true);
	textures.push_back(texture);
}

void cParticle::addParticleInternal(const Vec2& posInput, sol::table* paramsP, cParticleRandomizer* randomizer, bool replaceLastParticle)
{
	extern bool disableAddParticle;
	if (disableAddParticle)
	{
		return;
	}
	if (disableParticle)
	{
		return;
	}

#ifdef SHOW_TIMINGS
	glFinish();
	static cAccumulatedTimeProfiler& p = Coral::createAccumulatedTimeProfile("cParticle::addParticleInternal");
	p.start();
#endif

	
	Vec2 pos = posInput;
	if (replaceLastParticle == false)
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
				bufferData.quadBuffer = particleTemplate->emptyBuffers[particleTemplate->emptyBuffers.size() - 1];
				particleTemplate->emptyBuffers.resize(particleTemplate->emptyBuffers.size() - 1);
			}


			quadBuffers.push_back(bufferData);
		}
	}

#ifdef SHOW_TIMINGS
	glFinish();
	static cAccumulatedTimeProfiler& p2 = Coral::createAccumulatedTimeProfile("cParticle::addParticleInternal 1");
	p2.start();
#endif


	QuadBufferData &bufferData = quadBuffers[quadBuffers.size() - 1];

	if (particleTemplate->needsLuaRandoms && paramsP)
	{
		(*paramsP)["particleBeginTime"] = time;
		particleTemplate->scriptTable["addParticle"](*paramsP, pos, args);

	}
	int particleNumToAdd = particleTemplate->isStrip ? 2 : 4;

	memset(buff, 0, particleTemplate->attributeSize * particleNumToAdd);

	int vertexSize = particleTemplate->attributeSize;

	cVector<int> setAttributes;
	setAttributes.resize(shader->getAttributeCount());
	for (int i=0; i<setAttributes.size(); i++)
	{
		setAttributes[i] = shader->getAttributeAtIndex(i).location == -1 ? 1 : 0;
	}

	if (paramsP)
	{
		sol::table& params = *paramsP;

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

				setAttributes[attribute.index] = 1;
			}
			else if (attribute.name == "time")
			{
			}
			else if (attribute.name == "uv")
			{
			}
			else
			{
				//assert(!"attribute is not set");
			}
		}
	}

#ifdef SHOW_TIMINGS
	glFinish();
	static cAccumulatedTimeProfiler& p3 = Coral::createAccumulatedTimeProfile("cParticle::addParticleInternal 3");
	p3.start();
#endif

	setAttributes[particleTemplate->attributes[particleTemplate->posIndex].index] = 1;
	*(Vec2*)(buff + particleTemplate->attributes[particleTemplate->posIndex].begin) = pos;
	setAttributes[particleTemplate->attributes[particleTemplate->timeIndex].index] = 1;
	*(float*)(buff + particleTemplate->attributes[particleTemplate->timeIndex].begin) = timer.getTime() - time;

	setAttributes[particleTemplate->attributes[particleTemplate->uvIndex].index] = 1;

	particleTemplate->randomizeAttributes(particleTemplate->randomizer, this, pos, buff, setAttributes);
	particleTemplate->randomizeAttributes(this->randomizer, this, pos, buff, setAttributes);
	if (randomizer)
	{
		particleTemplate->randomizeAttributes(*randomizer, this, pos, buff, setAttributes);
	}

	for (int i = 0; i < setAttributes.size(); i++)
	{
		if (setAttributes[i] == 0)
		{
			out << "Attribute not set :" << shader->getAttributeAtIndex(i).name << "\n";
		}
	}

	if (particleNumToAdd == 4)
	{
		memcpy(buff + vertexSize * 1, buff, vertexSize);
		memcpy(buff + vertexSize * 2, buff, vertexSize);
		memcpy(buff + vertexSize * 3, buff, vertexSize);

		if (particleTemplate->textures.size())
		{
			*(Vec2*)(buff + vertexSize * 0 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 0.0f);
			*(Vec2*)(buff + vertexSize * 1 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 1.0f);
			*(Vec2*)(buff + vertexSize * 2 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 1.0f);
			*(Vec2*)(buff + vertexSize * 3 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 0.0f);
		}
	}
	else
	{
		memcpy(buff + vertexSize * 1, buff, vertexSize);

		if (replaceLastParticle == false)
		{
			nextIsStripBegining = !nextIsStripBegining;
		}

		if (!nextIsStripBegining)
		{
			*(Vec2*)(buff + vertexSize * 0 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 0.0f);
			*(Vec2*)(buff + vertexSize * 1 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 1.0f);
		}
		else
		{
			*(Vec2*)(buff + vertexSize * 0 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 0.0f);
			*(Vec2*)(buff + vertexSize * 1 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 1.0f);
		}
	}

#ifdef SHOW_TIMINGS
	glFinish();
	static cAccumulatedTimeProfiler& p4 = Coral::createAccumulatedTimeProfile("cParticle::addParticleInternal 4");
	p4.start();
#endif

	glBindBuffer(GL_ARRAY_BUFFER, bufferData.quadBuffer);

#ifdef SHOW_TIMINGS
	glFinish();
	static cAccumulatedTimeProfiler& p5 = Coral::createAccumulatedTimeProfile("cParticle::addParticleInternal 5");
	p5.start();
#endif

	if (replaceLastParticle)
	{
		glBufferSubData(GL_ARRAY_BUFFER, vertexSize * ( bufferData.count - particleNumToAdd), vertexSize * particleNumToAdd, buff);
		bufferData.timeToDie = timer.getTime() + particleTemplate->maxLifeTime;
	}
	else
	{
		glBufferSubData(GL_ARRAY_BUFFER, vertexSize * bufferData.count, vertexSize * particleNumToAdd, buff);

		bufferData.count += particleNumToAdd;
		bufferData.timeToDie = timer.getTime() + particleTemplate->maxLifeTime;
	}

#ifdef SHOW_TIMINGS
	glFinish();
	static cAccumulatedTimeProfiler& p6 = Coral::createAccumulatedTimeProfile("cParticle::render 6");
	p6.start();
#endif

#ifdef SHOW_TIMINGS
	glFinish();
	p.stop();
	p2.stop();
	p3.stop();
	p4.stop();
	p5.stop();
	p6.stop();
#endif
}

void cParticle::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	extern bool disableParticleRender;
	if (disableParticleRender)
	{
		return;
	}

#ifdef SHOW_TIMINGS
	glFinish();
	static cAccumulatedTimeProfiler& p = Coral::createAccumulatedTimeProfile("cParticle::render");
	p.start();
#endif


	if (quadBuffers.size() > 0)
	{
		particleTemplate->setShaderUniforms();

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

#ifdef SHOW_TIMINGS
		glFinish();
		static cAccumulatedTimeProfiler& p2 = Coral::createAccumulatedTimeProfile("cParticle::render 2");
		p2.start();
#endif

		game->lastShader = particleTemplate->shader;
		particleTemplate->shader->begin();
		particleTemplate->shader->setUniform(particleTemplate->uCurrentTime, timer.getTime() - time);

		cRenderableWithShader::render(isIdentity, mat, crop);

#ifdef SHOW_TIMINGS
		glFinish();
		static cAccumulatedTimeProfiler& p3 = Coral::createAccumulatedTimeProfile("cParticle::render 3");
		p3.start();
#endif

		glActiveTexture(GL_TEXTURE0);

		for (int i = textures.size() - 1; i >= 0; i--)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i]->bindTexture();
			particleTemplate->shader->setTexture(i, i);
		}

		particleTemplate->shader->setViewMatrix(game->getViewMatrix(alignment));

#ifdef SHOW_TIMINGS
		glFinish();
		static cAccumulatedTimeProfiler& p4 = Coral::createAccumulatedTimeProfile("cParticle::render 4");
		p4.start();
#endif

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

#ifdef SHOW_TIMINGS
		glFinish();
		static cAccumulatedTimeProfiler& p5 = Coral::createAccumulatedTimeProfile("cParticle::render 5");
		p5.start();
#endif


#ifdef SHOW_TIMINGS
		glFinish();
		p2.stop();
		p3.stop();
		p4.stop();
		p5.stop();
#endif

	}
#ifdef SHOW_TIMINGS
	glFinish();
	p.stop();
#endif
}

void cParticleTemplate::randomizeAttributes(cParticleRandomizer& randomizer, cParticle* particle, Vec2& pos, char *buff, cVector<int>& setAttributes) const
{
	for (auto& r : randomizer.linearRandoms)
	{
		assert(attributes[r.attributeIndex].type == TypeFloat);
		*(float*)(buff + attributes[r.attributeIndex].begin) = randFloat(r.min, r.max);
		setAttributes[attributes[r.attributeIndex].index] = 1;
	}

	for (auto& r : randomizer.angularRandoms)
	{
		assert(attributes[r.attributeIndex].type == TypeVec2);
		*(Vec2*)(buff + attributes[r.attributeIndex].begin) = Vec2::fromAngle(randFloat(r.minAngle, r.maxAngle)) * randFloat(r.min, r.max);
		setAttributes[attributes[r.attributeIndex].index] = 1;
	}

	for (auto& r : randomizer.textureRandoms)
	{
		assert(0);
	}

	for (auto& r : randomizer.vectorRandoms)
	{
		assert(attributes[r.attributeIndex].type == TypeVec2 || attributes[r.attributeIndex].type == TypeVec3 || attributes[r.attributeIndex].type == TypeVec4);
		Vec4 color;
		color[0] = randFloat(r.min[0], r.max[0]);
		color[1] = randFloat(r.min[1], r.max[1]);
		color[2] = randFloat(r.min[2], r.max[2]);
		color[3] = randFloat(r.min[3], r.max[3]);

		if (attributes[r.attributeIndex].type == TypeVec2)
		{
			*(Vec2*)(buff + attributes[r.attributeIndex].begin) = color.vec2;
		}
		else if (attributes[r.attributeIndex].type == TypeVec3)
		{
			*(Vec3*)(buff + attributes[r.attributeIndex].begin) = color.vec3;
		}
		else
		{
			*(Vec4*)(buff + attributes[r.attributeIndex].begin) = color;
		}
		setAttributes[attributes[r.attributeIndex].index] = 1;
	}

	for (auto& r : randomizer.uvRandoms)
	{
		assert(attributes[r.attributeIndexStart].type == TypeVec2);
		assert(attributes[r.attributeIndexSize].type == TypeVec2);

		Vec2 size;

		size[0] = randFloat(r.min[0], r.max[0]);
		size[1] = randFloat(r.min[1], r.max[1]);
		Vec2 start;

		start[0] = randFloat(1.0f - size[0]);
		start[1] = randFloat(1.0f - size[1]);

		*(Vec2*)(buff + attributes[r.attributeIndexStart].begin) = start;
		*(Vec2*)(buff + attributes[r.attributeIndexSize].begin) = size;

		setAttributes[attributes[r.attributeIndexStart].index] = 1;
		setAttributes[attributes[r.attributeIndexSize].index] = 1;
	}

	if (randomizer.positionRandom.isValid)
	{
		if (randomizer.positionRandom.isLinear)
		{
			pos[0] += randFloat(randomizer.positionRandom.min, randomizer.positionRandom.max);
			pos[1] += randFloat(randomizer.positionRandom.min, randomizer.positionRandom.max);
		}
		else
		{

			pos += Vec2::fromAngle(randFloat(randomizer.positionRandom.minAngle, randomizer.positionRandom.maxAngle)) * randFloat(randomizer.positionRandom.min, randomizer.positionRandom.max);
		}
		*(Vec2*)(buff + attributes[posIndex].begin) = pos;

		setAttributes[attributes[posIndex].index] = 1;
	}
}
