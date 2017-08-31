#pragma once

#include "cTools.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <GL/glew.h>

enum {
	TypeFloat,
	TypeVec2,
	TypeVec3,
	TypeVec4,
	TypeMat2,
	TypeMat3,
	TypeMat4,
	TypeInt,
	TypeIntVec2,
	TypeIntVec3,
	TypeIntVec4,
};

class cShader 
{
public:
	class Uniform 
	{
	public:
		int index;

		int uniformType;

		GLint location;

		Uniform(int uniformType) 
		{
			this->uniformType = uniformType;
		}

		Uniform() {}

		void setData(void * data)  const
		{
			if (location == -1)
			{
				return;
			}

			switch (uniformType) 
			{
			case TypeFloat:
				glUniform1fv(location, 1, (GLfloat*)data);
				break;
			case TypeVec2:
				glUniform2fv(location, 1, (GLfloat*)data);
				break;
			case TypeVec3:
				glUniform3fv(location, 1, (GLfloat*)data);
				break;
			case TypeVec4:
				glUniform4fv(location, 1, (GLfloat*)data);
				break;
			case TypeMat2:
				glUniformMatrix2fv(location, 1, false, (GLfloat*)data);
				break;
			case TypeMat3:
				glUniformMatrix3fv(location, 1, false, (GLfloat*)data);
				break;
			case TypeMat4:
				glUniformMatrix4fv(location, 1, false, (GLfloat*)data);
				break;
			case TypeInt:
				glUniform1iv(location, 1, (GLint*)data);
				break;
			case TypeIntVec2:
				glUniform2iv(location, 1, (GLint*)data);
				break;
			case TypeIntVec3:
				glUniform3iv(location, 1, (GLint*)data);
				break;
			case TypeIntVec4:
				glUniform4iv(location, 1, (GLint*)data);
				break;
			default:
				break;
			}
		}
	};

	class Attribute 
	{
	public:
		std::string name;
		int index;
		int attributeType;
		bool normalized;
		GLint location;

		Attribute(int attributeType, bool normalized = false) 
		{
			this->attributeType = attributeType;
			this->normalized = normalized;
		}

		Attribute() {}

		bool isNormalized() const
		{
			return normalized;
		}

		GLsizei getCount()  const
		{
			switch (attributeType) 
			{
			case TypeInt:
			case TypeFloat:
				return 1;
			case TypeVec2:
			case TypeIntVec2:
				return 2;
			case TypeVec3:
			case TypeIntVec3:
				return 3;
				break;
			case TypeVec4:
			case TypeIntVec4:
				return 4;
			default:
				assert(0);
				return 0;
			}
		}

		GLenum getType() const
		{
			switch (attributeType) 
			{
			case TypeFloat:
			case TypeVec2:
			case TypeVec3:
			case TypeVec4:
				return GL_FLOAT;
			case TypeInt:
			case TypeIntVec2:
			case TypeIntVec3:
			case TypeIntVec4:
				return GL_INT;
			default:
				assert(0);
				return 0;
			}
		}

		void setData(void * data) 
		{
			switch (attributeType) 
			{
			case TypeFloat:
				glVertexAttrib1fv(location, (GLfloat*)data);
				break;
			case TypeVec2:
				glVertexAttrib2fv(location, (GLfloat*)data);
				break;
			case TypeVec3:
				glVertexAttrib3fv(location, (GLfloat*)data);
				break;
			case TypeVec4:
				glVertexAttrib4fv(location, (GLfloat*)data);
				break;
			case TypeInt:
				glVertexAttribI1iv(location, (GLint*)data);
				break;
			case TypeIntVec2:
				glVertexAttribI2iv(location, (GLint*)data);
				break;
			case TypeIntVec3:
				glVertexAttribI3iv(location, (GLint*)data);
				break;
			case TypeIntVec4:
				glVertexAttribI4iv(location, (GLint*)data);
				break;
			default:
				assert(0);
				break;
			}
		}
	};

	private:
	GLuint vertexShader;
	GLuint pixelShader;
	std::string vertexShaderFile;
	std::string pixelShaderFile;

	void printShaderInfoLog(const std::string& name, GLuint obj)
	{
		int infologLength = 0;
		int charsWritten = 0;
		char *infoLog;

		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 1) {
			infoLog = (char *)malloc(infologLength);
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("%s\n%s\n\n", name.c_str(), infoLog);
			free(infoLog);
		}
	}

	void printProgramInfoLog(GLuint obj) 
	{
		int infologLength = 0;
		int charsWritten = 0;
		char *infoLog;

		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 1) {
			infoLog = (char *)malloc(infologLength);
			glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("%s\n", infoLog);
			free(infoLog);
		}
	}


	GLuint buildShader(const std::string& name, const std::string& source, int shaderType)
	{
		assert(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER);


		GLuint shader = glCreateShader(shaderType);
		const char* cSource = source.c_str();
		glShaderSource(shader, 1, &cSource, NULL);

		glCompileShader(shader);
		int compileStatus;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

		if (!compileStatus) 
		{
			printShaderInfoLog(name, shader);
			return 0;
		}

		return shader;
	}

	GLuint loadShader(const std::string& fileName, int shaderType)
	{
		std::string shaderSource;
		if (!textFileRead(fileName, shaderSource)) {
			std::cout << "Cannot load shader file : " << fileName;
			return 0;
		}
		return buildShader(fileName, shaderSource, shaderType);
	}
	GLuint shaderProgram;

	std::unordered_map<std::string, int> uniformIndices;
	std::unordered_map<std::string, int> attributeIndices;

	cVector<Uniform> uniforms;
	cVector<Attribute> attributes;

	int aVertexPosition;
	int aVertexColor;
	int aVertexUV;

	int uWorldMatrix;
	int uViewMatrix;
	int uColor;
	int uTextures[4];
	int uCrop;
public:
	cShader();
	~cShader();

	const std::string& getVertexShaderFileName() const 
	{
		return vertexShaderFile;
	}

	const std::string& getPixelShaderFileName() const
	{
		return pixelShaderFile;
	}

	bool loadFromFile(const std::string& vertexShaderFile, const std::string& pixelShaderFile)
	{
		printf("loading %s %s\n", vertexShaderFile.c_str(), pixelShaderFile.c_str());
		this->vertexShaderFile = vertexShaderFile;
		this->pixelShaderFile = pixelShaderFile;

		vertexShader = loadShader(vertexShaderFile, GL_VERTEX_SHADER);
		if (!vertexShader) 
		{
			return false;
		}
		pixelShader = loadShader(pixelShaderFile, GL_FRAGMENT_SHADER);
		if (!pixelShader) 
		{
			return false;
		}

		shaderProgram = glCreateProgram();
		if (!shaderProgram) 
		{
			return false;
		}
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, pixelShader);

		glLinkProgram(shaderProgram);

		int linkStatus;
		glGetShaderiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

		if (!linkStatus)
			return false;

		glUseProgram(shaderProgram);
		addDefaultUniforms();
		addDefaultAttribs();

		return true;
	}

	void begin() 
	{
		glUseProgram(shaderProgram);
	}

	void addDefaultAttribs() 
	{
		aVertexPosition = addAttribute("aVertexPosition", TypeVec2).index;
		aVertexColor = addAttribute("aVertexColor", TypeVec4).index;
		aVertexUV = addAttribute("aVertexUV", TypeVec2).index;
	}

	void bindPosition(int stride, int pointer) 
	{
		bindAttribute(aVertexPosition, stride, pointer);
	}

	void bindColor(int stride, int pointer) 
	{
		bindAttribute(aVertexColor, stride, pointer);
	}

	void setAttributeColor(const Vec4& color) 
	{
		setAttributeConstant(aVertexColor, color);
	}

	void bindUV(int stride, int pointer) 
	{
		bindAttribute(aVertexUV, stride, pointer);
	}

	void setAttributeUV(const Vec2& uv) 
	{
		setAttributeConstant(aVertexUV, uv);
	}

	void bindAttribute(int index, int stride, int pointer)
	{
		Attribute& attribute = attributes[index];
		glEnableVertexAttribArray(attribute.location);
		glVertexAttribPointer(attribute.location, attribute.getCount(), attribute.getType(), attribute.isNormalized(), stride, (const void*)(long long)pointer);
	}


	Attribute addAttribute(const std::string& name, int attributeType, bool normalized = false, int attributeLocation = -1)  // todo Attribute shouldn't be public
	{
		if (attributeIndices.count(name))
		{
			return attributes[attributeIndices[name]];
		}

		Attribute attribute(attributeType, normalized);
		attribute.name = name;
		if (attributeLocation == -1) 
		{
			attribute.location = glGetAttribLocation(shaderProgram, name.c_str());
		}
		else 
		{
			attribute.location = attributeLocation;
			glBindAttribLocation(shaderProgram, attributeLocation, name.c_str());
		}
		attribute.index = attributeIndices[name] = attributes.size();
		attributes.push_back(attribute);
		return attributes[attribute.index];
	}

	const Attribute& getAttributeAtIndex(int index)
	{
		return attributes[index];
	}

	int getTotalAttributeSize()
	{
		int size = 0;
		for (auto& attribute : attributes)
		{
			size += attribute.getCount() * 4;
		}
		return size;
	}

	void addDefaultUniforms() 
	{
		uWorldMatrix = addUniform("uWorldMatrix", TypeMat3).index;
		uViewMatrix = addUniform("uViewMatrix", TypeMat3).index;
		uColor = addUniform("uColor", TypeVec4).index;
		uTextures[0] = addUniform("uTexture0", TypeInt).index;
		uTextures[1] = addUniform("uTexture1", TypeInt).index;
		uTextures[2] = addUniform("uTexture2", TypeInt).index;
		uTextures[3] = addUniform("uTexture3", TypeInt).index;
		uCrop = addUniform("uCrop", TypeVec4).index;
	}

	Uniform addUniform(const std::string& name, int uniformType) // todo Uniform shouldn't be public
	{
		if (uniformIndices.count(name))
		{
			return uniforms[uniformIndices[name]];
		}

		Uniform uniform(uniformType);
		uniform.index = uniformIndices[name] = uniforms.size();
		uniform.location = glGetUniformLocation(shaderProgram, name.c_str());
		uniforms.push_back(uniform);
		return uniforms[uniform.index];
	}

	void setCrop(const AARect& crop)
	{
		setUniform(uniforms[uCrop], crop.toVec());
	}

	void setViewMatrix(const Mat3& mat)
	{
		setUniform(uniforms[uViewMatrix], mat);
	}

	void setWorldMatrix(const Mat3& mat)
	{
		setUniform(uniforms[uWorldMatrix], mat);
	}

	void setColor(const Vec4& color) 
	{
		setUniform(uniforms[uColor], color);
	}

	void setTexture0(int texture)
	{
		setUniform(uniforms[uTextures[0]], texture);
	}
	void setTexture1(int texture)
	{
		setUniform(uniforms[uTextures[1]], texture);
	}
	void setTexture2(int texture)
	{
		setUniform(uniforms[uTextures[2]], texture);
	}
	void setTexture3(int texture)
	{
		setUniform(uniforms[uTextures[3]], texture);
	}
	void setTexture(int i, int texture)
	{
		setUniform(uniforms[uTextures[i]], texture);
	}

	void setUniform(int uniformIndex, void* data)
	{
		uniforms[uniformIndex].setData(data);
	}

	void setUniform(int uniformIndex, float data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const Vec2& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const Vec3& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const Vec4& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const Mat3& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const Mat2& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, int data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const IntVec2& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const IntVec3& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}

	void setUniform(int uniformIndex, const IntVec4& data)
	{
		uniforms[uniformIndex].setData((void*)&data);
	}


	void setUniform(const Uniform& uniform, void* data)
	{
		uniform.setData(data);
	}

	void setUniform(const Uniform& uniform, float data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const Vec2& data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const Vec3& data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const Vec4& data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const Mat3& data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const Mat2& data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, int data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const IntVec2& data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const IntVec3& data)
	{
		uniform.setData((void*)&data);
	}

	void setUniform(const Uniform& uniform, const IntVec4& data)
	{
		uniform.setData((void*)&data);
	}
	void setAttributeConstant(int index, void* data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData(data);
	}

	void setAttributeConstant(int index, float data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}

	void setAttributeConstant(int index, const Vec2& data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}

	void setAttributeConstant(int index, const Vec3& data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}

	void setAttributeConstant(int index, const Vec4& data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}

	void setAttributeConstant(int index, int data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}

	void setAttributeConstant(int index, const IntVec2& data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}

	void setAttributeConstant(int index, const IntVec3& data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}

	void setAttributeConstant(int index, const IntVec4& data)
	{
		glDisableVertexAttribArray(attributes[index].location);
		attributes[index].setData((void*)&data);
	}


	int getAttributeCount() const
	{
		return attributes.size();
	}

	void deleteSelf()
	{
		delete this;
	}


};

