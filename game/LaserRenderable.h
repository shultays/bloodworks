#pragma once

#include "cShader.h"
#include "cRenderableWithShader.h"
#include "json.h"

class Bloodworks;


class LaserTemplate
{
	std::string name;

	friend class LaserRenderable;
	cTextureShr laserTexture;
	cShaderShr shader;

	float laserThickness;
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

	cGame *game;
public:
	void render(float laserLength);
	LaserTemplate(cGame *game, nlohmann::json& j);
	~LaserTemplate();
	const std::string& getName() const;
};

class LaserRenderable : public cRenderableWithShader
{
	float laserLength;

	LaserTemplate *laserTemplate;
	float angle;
	Vec2 pos;

	void updateMatrix();
public:
	LaserRenderable(Bloodworks *bloodworks, LaserTemplate *laserTemplate);
	~LaserRenderable();
	void setLaserData(const Vec2& pos, float angle, float length);
	void setPositionAndAngle(const Vec2& pos, float angle);
	void setPosition(const Vec2& pos);
	void setAngle(float angle);
	void setLength(float length);
	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;
};