#include "cShader.h"

extern int totalResource;

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
		printf("unloading %s %s\n", vertexShaderFile.c_str(), pixelShaderFile.c_str());
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

