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
		out << "unloading " << vertexShaderFile << " " << pixelShaderFile << "\n";
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

