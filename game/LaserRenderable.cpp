#include "LaserRenderable.h"
#include "cTexture.h"
#include "cShader.h"
#include "cGame.h"


GLuint laserQuad = -1;

LaserRenderable::LaserRenderable(Bloodworks *bloodworks) : cRenderable((cGame*)bloodworks)
{
	laserLength = 100.0f;
	laserSize = 20.0f;

	laserBeginWidth = 10.0f;
	laserEndWidth = 10.0f;

	laserBeginShift = 0;
	laserEndShift = 0;
	

	laserTexture = resources.getTexture("resources/guns/laser/laser.png");
	shader = resources.getShader("resources/guns/laser/laser.vs", "resources/guns/laser/laser.ps");

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

void LaserRenderable::render(bool isIdentity, const Mat3& mat)
{
	game->lastShader = nullptr;
	glEnable(GL_TEXTURE_2D);
	shader->begin();
	shader->setViewMatrix(game->getViewMatrix(RenderableAlignment::world));
	shader->setWorldMatrix(Mat3::identity());
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

	shader->setUniform(laserWidth.index, laserSize);

	glDrawArrays(GL_QUADS, 0, 12);
	/*
	glBindBuffer(GL_ARRAY_BUFFER, laserQuadMid);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, laserQuadEnd);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);*/
}
