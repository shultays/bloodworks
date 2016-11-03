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

class cShader {
	class Uniform {
	public:
		int uniformType;

		GLint location;

		Uniform(int uniformType) 
		{
			this->uniformType = uniformType;
		}

		Uniform() {}

		void setData(void * data) 
		{
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
		int attributeType;
		bool normalized;
		GLint location;

		Attribute(int attributeType, bool normalized = false) 
		{
			this->attributeType = attributeType;
			this->normalized = normalized;
		}

		Attribute() {}

		bool isNormalized() 
		{
			return normalized;
		}

		GLsizei getCount() 
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

		GLenum getType() 
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

	GLuint vertexShader;
	GLuint pixelShader;
	char vertexShaderFile[64];
	char pixelShaderFile[64];

	void printShaderInfoLog(GLuint obj) 
	{
		int infologLength = 0;
		int charsWritten = 0;
		char *infoLog;

		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 1) {
			infoLog = (char *)malloc(infologLength);
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
			printf("%s\n", infoLog);
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


	GLuint buildShader(const std::string& source, int shaderType) 
	{
		assert(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER);


		GLuint shader = glCreateShader(shaderType);
		const char* cSource = source.c_str();
		glShaderSource(shader, 1, &cSource, NULL);

		glCompileShader(shader);
		int compileStatus;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

		if (!compileStatus) {
			printShaderInfoLog(shader);
			return 0;
		}

		return shader;
	}

	GLuint loadShader(const char *fileName, int shaderType) 
	{
		std::string shaderSource;
		if (!textFileRead(fileName, shaderSource)) {
			std::cout << "Cannot load shader file : " << fileName;
			return 0;
		}
		return buildShader(shaderSource, shaderType);
	}
public:
	GLuint shaderProgram;

	std::unordered_map<std::string, Uniform> uniforms;
	std::unordered_map<std::string, Attribute> attributes;

	cShader() {

	}

	const char* getVertexShaderFileName() const 
	{
		return vertexShaderFile;
	}

	const char* getPixelShaderFileName() const 
	{
		return pixelShaderFile;
	}

	bool loadFromFile(const char* vertexShaderFile, const char* pixelShaderFile) 
	{
		strcpy_s(this->vertexShaderFile, vertexShaderFile);
		strcpy_s(this->pixelShaderFile, pixelShaderFile);

		vertexShader = loadShader(vertexShaderFile, GL_VERTEX_SHADER);
		if (!vertexShader) {
			return false;
		}
		pixelShader = loadShader(pixelShaderFile, GL_FRAGMENT_SHADER);
		if (!pixelShader) {
			return false;
		}

		shaderProgram = glCreateProgram();
		if (!shaderProgram) {
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
		addAttribute("aVertexPosition", TypeVec2);
		addAttribute("aVertexColor", TypeVec4);
		addAttribute("aVertexUV", TypeVec2);
	}


	void bindPosition(int stride, int pointer) 
	{
		bindAttribute("aVertexPosition", stride, pointer);
	}

	void bindColor(int stride, int pointer) 
	{
		bindAttribute("aVertexColor", stride, pointer);
	}

	void setAttributeColor(const Vec4& color) 
	{
		setAttributeConstant("aVertexColor", color);
	}

	void bindUV(int stride, int pointer) 
	{
		bindAttribute("aVertexUV", stride, pointer);
	}

	void setAttributeUV(const Vec2& uv) 
	{
		setAttributeConstant("aVertexUV", uv);
	}


	void bindAttribute(char* name, int stride, int pointer) 
	{
		Attribute& attribute = attributes.at(name);
		glEnableVertexAttribArray(attribute.location);
		glVertexAttribPointer(attribute.location, attribute.getCount(), attribute.getType(), attribute.isNormalized(), stride, (const void*)(long long)pointer);
	}


	void addAttribute(char* name, int attributeType, bool normalized = false, int attributeLocation = -1) 
	{
		Attribute& attribute = attributes[name] = Attribute(attributeType, normalized);
		if (attributeLocation == -1) {
			attribute.location = glGetAttribLocation(shaderProgram, name);
		}
		else {
			attribute.location = attributeLocation;
			glBindAttribLocation(shaderProgram, attributeLocation, name);
		}
	}

	void addDefaultUniforms() 
	{
		addUniform("uWorldMatrix", TypeMat3);
		addUniform("uViewMatrix", TypeMat3);
		addUniform("uColor", TypeVec4);
		addUniform("uTexture", TypeInt);
	}

	void addUniform(char* name, int uniformType) 
	{
		Uniform& uniform = uniforms[name] = Uniform(uniformType);
		uniform.location = glGetUniformLocation(shaderProgram, name);
	}

	void setViewMatrix(const Mat3& mat)
	{
		setUniform("uViewMatrix", mat);
	}

	void setWorldMatrix(const Mat3& mat)
	{
		setUniform("uWorldMatrix", mat);
	}

	void setColor(const Vec4& color) 
	{
		setUniform("uColor", color);
	}

	void setUniform(char* name, void* data) 
	{
		uniforms[name].setData(data);
	}

	void setUniform(char* name, float data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const Vec2& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const Vec3& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const Vec4& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const Mat3& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const Mat2& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, int data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const IntVec2& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const IntVec3& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setUniform(char* name, const IntVec4& data) 
	{
		uniforms[name].setData((void*)&data);
	}

	void setAttributeConstant(char* name, void* data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData(data);
	}

	void setAttributeConstant(char* name, float data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void setAttributeConstant(char* name, const Vec2& data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void setAttributeConstant(char* name, const Vec3& data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void setAttributeConstant(char* name, const Vec4& data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void setAttributeConstant(char* name, int data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void setAttributeConstant(char* name, const IntVec2& data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void setAttributeConstant(char* name, const IntVec3& data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void setAttributeConstant(char* name, const IntVec4& data) 
	{
		glDisableVertexAttribArray(attributes.at(name).location);
		attributes.at(name).setData((void*)&data);
	}

	void deleteSelf()
	{
		delete this;
	}
};

