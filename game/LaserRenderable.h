#pragma once

#include "cShader.h"
#include "cRenderable.h"
#include "json.h"

class Bloodworks;

class LaserRenderable : public cRenderable
{
	cTextureShr laserTexture;
	cShaderShr shader;

	float laserLength;
	float laserSize;

	float laserBeginShift;
	float laserBeginWidth;
	float laserEndShift;
	float laserEndWidth;

	cShader::Attribute aYShift;
	cShader::Attribute widthMult1;
	cShader::Attribute widthMult2;
	cShader::Attribute widthMult3;
	cShader::Attribute xUV;

	cShader::Uniform beginX;
	cShader::Uniform width1;
	cShader::Uniform width2;
	cShader::Uniform width3;
	cShader::Uniform laserWidth;
public:
	LaserRenderable(Bloodworks *bloodworks);
	~LaserRenderable();
	virtual void render(bool isIdentity, const Mat3& mat) override;
};