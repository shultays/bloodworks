#include "cParticle.h"
#include "cGlobals.h"
#include "cShader.h"
#include "cGame.h"
#include "cTexture.h"
#include "cTools.h"
#include "cTools.h"
#include "cTimeProfiler.h"

#define MAX_VERTEX 512 * 6

bool disableParticle;
bool staticParticleBuffersG = true;

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

	addParticle = scriptTable["addParticle"];

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

cParticleTemplate::~cParticleTemplate()
{
	shader = nullptr;
	for (auto& t : textures)
	{
		t = nullptr;
	}
	textures.clear();
	for (auto& t : emptyBuffers)
	{
		EmptyBuffer& b = t;
		glDeleteBuffers(1, &b.arrayBuffer);
		glDeleteVertexArrays(1, &b.vertexBuffer);
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

	this->args = args;
	maxBufferSize = 0;
	nextIsStripBegining = true;

	if (particleTemplate->scriptTable["setDefaultArgs"])
	{
		particleTemplate->scriptTable["setDefaultArgs"](args);
	}
	textures = particleTemplate->textures;

	dataToPush.reserve(particleTemplate->attributeSize * 4 * 16);
	lastDirtyCount = -1;

	staticParticleBuffers = staticParticleBuffersG && !particleTemplate->isStrip;
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

cParticle::~cParticle()
{
	textures.clear();
	clear();

	if (dataToPush.size())
	{
		coral.removeDirtyParticle(this);
	}
}

void cParticle::addParticleInternal(const Vec2& posInput, sol::table* paramsP, cParticleRandomizer* randomizer, bool replaceLastParticle)
{
	if (disableParticle)
	{
		return;
	}

	Vec2 pos = posInput;
	if (replaceLastParticle)
	{
		pushData();
	}

	int particleNumToAdd = particleTemplate->isStrip ? 2 : 6;
	int dataSize = particleTemplate->attributeSize * particleNumToAdd;

	int oldSize = dataToPush.size();
	dataToPush.resize(oldSize + dataSize);

	char* buff = &dataToPush[0] + oldSize;

	memset(buff, 0, dataSize);

	int vertexSize = particleTemplate->attributeSize;

	if (particleTemplate->needsLuaRandoms && paramsP)
	{
		(*paramsP)["particleBeginTime"] = time;
		particleTemplate->addParticle(*paramsP, pos, args);
	}

	cVector<int> setAttributes;
	setAttributes.resize(shader->getAttributeCount());
	for (int i = 0; i < setAttributes.size(); i++)
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

	if (particleNumToAdd == 6)
	{
		memcpy(buff + vertexSize * 1, buff, vertexSize);
		memcpy(buff + vertexSize * 2, buff, vertexSize);
		memcpy(buff + vertexSize * 3, buff, vertexSize);
		memcpy(buff + vertexSize * 4, buff, vertexSize);
		memcpy(buff + vertexSize * 5, buff, vertexSize);

		if (particleTemplate->textures.size())
		{
			*(Vec2*)(buff + vertexSize * 0 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 0.0f);
			*(Vec2*)(buff + vertexSize * 1 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 1.0f);
			*(Vec2*)(buff + vertexSize * 2 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 1.0f);

			*(Vec2*)(buff + vertexSize * 3 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 1.0f);
			*(Vec2*)(buff + vertexSize * 4 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(1.0f, 0.0f);
			*(Vec2*)(buff + vertexSize * 5 + particleTemplate->attributes[particleTemplate->uvIndex].begin) = Vec2(0.0f, 0.0f);
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

	if (replaceLastParticle)
	{
		QuadBufferData &bufferData = quadBuffers.back();

		glBindBuffer(GL_ARRAY_BUFFER, bufferData.arrayBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, vertexSize * (bufferData.count - particleNumToAdd), vertexSize * particleNumToAdd, buff);
		bufferData.timeToDie = timer.getTime() + particleTemplate->maxLifeTime;
		dataToPush.resize(dataToPush.size() - dataSize);
	}

	if (lastDirtyCount != coral.getDirtyParticleIndex())
	{
		lastDirtyCount = coral.getDirtyParticleIndex();
		coral.addDirtyParticle(this);
	}
}

void cParticle::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{

	if (quadBuffers.size() > 0 || dataToPush.size() > 0)
	{
		particleTemplate->setShaderUniforms();

		auto i = quadBuffers.begin();
		while (i != quadBuffers.end())
		{
			if (i->timeToDie < timer.getTime())
			{
				if (staticParticleBuffers)
				{
					glDeleteBuffers(1, &(i->arrayBuffer));
					glDeleteVertexArrays(1, &(i->vertexBuffer));
					quadBuffers.erase(i++);
				}
				else
				{
					if (particleTemplate->emptyBuffers.size() < 50)
					{
						cParticleTemplate::EmptyBuffer emptyBuffer;
						emptyBuffer.arrayBuffer = i->arrayBuffer;
						emptyBuffer.vertexBuffer = i->vertexBuffer;
						particleTemplate->emptyBuffers.push_back(emptyBuffer);
						quadBuffers.erase(i++);
					}
					else
					{
						glDeleteBuffers(1, &(i->arrayBuffer));
						glDeleteVertexArrays(1, &(i->vertexBuffer));
						quadBuffers.erase(i++);
					}
				}
			}
			else
			{
				break;
			}
		}

		game->lastShader = particleTemplate->shader;
		particleTemplate->shader->begin();
		particleTemplate->shader->setUniform(particleTemplate->uCurrentTime, timer.getTime() - time);

		cRenderableWithShader::render(isIdentity, mat, crop);

		glActiveTexture(GL_TEXTURE0);

		for (int i = textures.size() - 1; i >= 0; i--)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i]->bindTexture();
			particleTemplate->shader->setTexture(i, i);
		}

		particleTemplate->shader->setViewMatrix(game->getViewMatrix(alignment));

		{
			pushData();
		}

		for (auto& bufferData : quadBuffers)
		{
			if (bufferData.count >= 4)
			{
				glBindBuffer(GL_ARRAY_BUFFER, bufferData.arrayBuffer);
				glBindVertexArray(bufferData.vertexBuffer);

				{
					for (auto& attribute : particleTemplate->attributes)
					{
						particleTemplate->shader->bindAttribute(attribute.index, particleTemplate->attributeSize, attribute.begin);
					}
				}

				{
					if (particleTemplate->isStrip)
					{
						glDrawArrays(GL_TRIANGLE_STRIP, 0, bufferData.count);
					}
					else
					{
						glDrawArrays(GL_TRIANGLES, 0, bufferData.count);
					}
				}
			}
		}

		glDisableVertexAttribArray(0);

		game->lastShader = nullptr;
	}
}

void cParticle::clear()
{
	for (auto& bufferData : quadBuffers)
	{
		if (staticParticleBuffers)
		{
			glDeleteBuffers(1, &bufferData.arrayBuffer);
			glDeleteVertexArrays(1, &bufferData.vertexBuffer);
		}
		else if (particleTemplate->emptyBuffers.size() < maxBufferSize * 2)
		{
			cParticleTemplate::EmptyBuffer emptyBuffer;
			emptyBuffer.arrayBuffer = bufferData.arrayBuffer;
			emptyBuffer.vertexBuffer = bufferData.vertexBuffer;
			particleTemplate->emptyBuffers.push_back(emptyBuffer);
		}
		else
		{
			glDeleteBuffers(1, &bufferData.arrayBuffer);
			glDeleteVertexArrays(1, &bufferData.vertexBuffer);
		}
	}
	quadBuffers.clear();
}

void cParticle::pushData()
{
	int sizeStart = 0;

	while (sizeStart < dataToPush.size() )
	{
		if (staticParticleBuffers)
		{
			int sizeToPush = min(dataToPush.size() - sizeStart, 6 * 128 * particleTemplate->attributeSize );
			QuadBufferData bufferData;
			bufferData.count = 0;

			glGenBuffers(1, &bufferData.arrayBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, bufferData.arrayBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeToPush, &dataToPush[0] + sizeStart, GL_STATIC_DRAW);

			glGenVertexArrays(1, &bufferData.vertexBuffer);
			glBindVertexArray(bufferData.vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, bufferData.arrayBuffer);

			bufferData.count += sizeToPush / particleTemplate->attributeSize;
			bufferData.timeToDie = timer.getTime() + particleTemplate->maxLifeTime;

			quadBuffers.push_back(bufferData);
			sizeStart += sizeToPush;
		}
		else
		{
			bool bind = false;
			if (quadBuffers.size() == 0 || quadBuffers.back().count == MAX_VERTEX)
			{
				QuadBufferData bufferData;
				bufferData.count = 0;

				if (particleTemplate->emptyBuffers.size() == 0)
				{
					glGenBuffers(1, &bufferData.arrayBuffer);
					glBindBuffer(GL_ARRAY_BUFFER, bufferData.arrayBuffer);
					glBufferData(GL_ARRAY_BUFFER, particleTemplate->attributeSize * MAX_VERTEX, NULL, GL_DYNAMIC_DRAW);

					glGenVertexArrays(1, &bufferData.vertexBuffer);
					glBindVertexArray(bufferData.vertexBuffer);
					glBindBuffer(GL_ARRAY_BUFFER, bufferData.arrayBuffer);
					bind = true;
				}
				else
				{
					bufferData.arrayBuffer = particleTemplate->emptyBuffers[particleTemplate->emptyBuffers.size() - 1].arrayBuffer;
					bufferData.vertexBuffer = particleTemplate->emptyBuffers[particleTemplate->emptyBuffers.size() - 1].vertexBuffer;

					particleTemplate->emptyBuffers.resize(particleTemplate->emptyBuffers.size() - 1);
				}


				quadBuffers.push_back(bufferData);
			}

			QuadBufferData& bufferData = quadBuffers.back();

			if (!bind)
			{
				glBindBuffer(GL_ARRAY_BUFFER, bufferData.arrayBuffer);
			}

			int emptySize = (MAX_VERTEX - bufferData.count) * particleTemplate->attributeSize;
			int sizeToPush = min(dataToPush.size() - sizeStart, emptySize);

			glBufferSubData(GL_ARRAY_BUFFER, particleTemplate->attributeSize * bufferData.count, sizeToPush, &dataToPush[0] + sizeStart);

			bufferData.count += sizeToPush / particleTemplate->attributeSize;
			bufferData.timeToDie = timer.getTime() + particleTemplate->maxLifeTime;

			sizeStart += sizeToPush;
		}
	}

	dataToPush.resize(0);
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
			*(Vec2*)(buff + attributes[r.attributeIndex].begin) = color.vec2();
		}
		else if (attributes[r.attributeIndex].type == TypeVec3)
		{
			*(Vec3*)(buff + attributes[r.attributeIndex].begin) = color.vec3();
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
