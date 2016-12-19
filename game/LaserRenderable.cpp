#include "LaserRenderable.h"
#include "cTexture.h"
#include "cShader.h"
#include "cGame.h"


GLuint laserQuad = -1;

LaserRenderable::LaserRenderable(Bloodworks *bloodworks, nlohmann::json& j) : cRenderable((cGame*)bloodworks)
{
	laserLength = 100.0f;

	laserThickness = j["laserThickness"].get<float>();

	laserBeginWidth = j["laserBeginWidth"].get<float>();
	laserEndWidth = j["laserEndWidth"].get<float>();

	laserBeginShift = j["laserBeginShift"].get<float>();
	laserEndShift = j["laserEndShift"].get<float>();
	
	laserTexture = resources.getTexture(j["laserTexture"].get<std::string>());
	shader = resources.getShader(j["laserShader"].get<std::string>());

	aYShift = shader->addAttribute("yShift", TypeFloat);
	widthMult1 = shader->addAttribute("widthMult1", TypeFloat);
	widthMult2 = shader->addAttribute("widthMult2", TypeFloat);
	widthMult3 = shader->addAttribute("widthMult3", TypeFloat);
	xUV = shader->addAttribute("xUV", TypeFloat);

	beginX = shader->addUniform("beginX", TypeFloat);
	width1 = shader->addUniform("width1", TypeFloat);;
	width2 = shader->addUniform("width2", TypeFloat);;
	width3 = shader->addUniform("width3", TypeFloat);;
	laserWidth = shader->addUniform("laserWidth", TypeFloat);;

	if (laserQuad == -1)
	{
		GLfloat vertexData[] =
		{
			-0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, 1.0f, 0.0f, 0.0f, 0.5f,
			0.5f, 1.0f, 0.0f, 0.0f, 0.5f,
			0.5f, 0.0f, 0.0f, 0.0f, 0.0f,

			-0.5f, 1.0f, 0.0f, 0.0f, 0.5f,
			-0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
			0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
			0.5f, 1.0f, 0.0f, 0.0f, 0.5f,

			-0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
			-0.5f, 1.0f, 1.0f, 1.0f, 1.0f,
			0.5f, 1.0f, 1.0f, 1.0f, 1.0f,
			0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
		};

		glGenBuffers(1, &laserQuad);
		glBindBuffer(GL_ARRAY_BUFFER, laserQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	}
}



LaserRenderable::~LaserRenderable()
{
	laserTexture = nullptr;
	shader = nullptr;
}

void LaserRenderable::setLaserData(const Vec2& pos, float angle, float length)
{
	Mat3 mat = Mat3::rotationMatrix(angle).translateBy(pos);
	laserLength = length;
	setWorldMatrix(mat);
}

void LaserRenderable::render(bool isIdentity, const Mat3& mat)
{
	game->lastShader = nullptr;
	glEnable(GL_TEXTURE_2D);
	shader->begin();
	shader->setViewMatrix(game->getViewMatrix(RenderableAlignment::world));
	shader->setWorldMatrix(worldMatrix);
	glActiveTexture(0);
	shader->setTexture0(0);
	laserTexture->bindTexture();

	glBindBuffer(GL_ARRAY_BUFFER, laserQuad);
	shader->bindAttribute(aYShift.index, 4 * 5, 4 * 0);
	shader->bindAttribute(widthMult1.index, 4 * 5,  4 * 1);
	shader->bindAttribute(widthMult2.index, 4 * 5,  4 * 2);
	shader->bindAttribute(widthMult3.index, 4 * 5,  4 * 3);
	shader->bindAttribute(xUV.index, 4 * 5, 4 * 4);


	shader->setUniform(beginX.index, laserBeginShift - laserBeginWidth);
	shader->setUniform(width1.index, laserBeginWidth);
	shader->setUniform(width2.index, laserLength + laserEndShift - laserBeginShift);
	shader->setUniform(width3.index, laserEndWidth);

	shader->setUniform(laserWidth.index, laserThickness);

	glDrawArrays(GL_QUADS, 0, 12);
	/*
	glBindBuffer(GL_ARRAY_BUFFER, laserQuadMid);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, laserQuadEnd);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);*/
}
