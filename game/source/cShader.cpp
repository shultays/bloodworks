#include "cShader.h"

extern int totalResource;

GLuint cShader::buildShader(const std::string& name, const std::string& source, int shaderType)
{
	assert(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER);


	GLuint shader = glCreateShader(shaderType);
	CHECK_GL_ERROR;

	const char* cSource = source.c_str();
	GL_CALL(glShaderSource(shader, 1, &cSource, NULL));

	GL_CALL(glCompileShader(shader));
	int compileStatus;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (!compileStatus)
	{
		printErrorLog(name, shader);
		return 0;
	}
	else
	{
#ifdef DEBUG
		printWarningsLog(name, shader);
#endif
	}

	return shader;
}

cShader::cShader()
{
	shaderProgram = pixelShader = vertexShader = -1;
	totalResource++;
}

cShader::~cShader()
{
	totalResource--;
	if (shaderProgram != -1)
	{
#ifdef DEBUG
		out << "unloading " << vertexShaderFile << " " << pixelShaderFile << "\n";
#endif
	}
	if (pixelShader != -1)
	{
		glDeleteShader(pixelShader);
	}
	if (vertexShader != -1)
	{
		glDeleteShader(vertexShader);
	}
	if (shaderProgram != -1)
	{
		glDeleteProgram(shaderProgram);
	}
}

bool cShader::loadFromFile(const std::string& vertexShaderFile, const std::string& pixelShaderFile)
{
	out << "loading " << vertexShaderFile << " " << pixelShaderFile << "\n";
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
	GL_CALL(glAttachShader(shaderProgram, vertexShader));
	GL_CALL(glAttachShader(shaderProgram, pixelShader));

	GL_CALL(glLinkProgram(shaderProgram));

	int linkStatus;
	GL_CALL(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus));

	if (!linkStatus)
		return false;

	GL_CALL(glUseProgram(shaderProgram));
	addDefaultUniforms();
	addDefaultAttribs();

	return true;
}

