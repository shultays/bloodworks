#include "LaserRenderable.h"
#include "cTexture.h"
#include "cShader.h"
#include "cGame.h"
#include "cGlobals.h"
#include "DirentHelper.h"

GLuint laserQuad = -1;
GLuint laserVertex = -1;

LaserRenderable::LaserRenderable(Bloodworks *bloodworks, LaserTemplate *laserTemplate) : cRenderableWithShader((cGame*)bloodworks, laserTemplate->shader)
{
	laserLength = 100.0f;
	this->laserTemplate = laserTemplate;
}

LaserRenderable::~LaserRenderable()
{
}

void LaserRenderable::setLaserData(const Vec2& pos, float angle, float length)
{
	this->pos = pos;
	this->angle = angle;
	laserLength = length;
	updateMatrix();
}

void LaserRenderable::setPositionAndAngle(const Vec2& pos, float angle)
{
	this->pos = pos;
	this->angle = angle;
	updateMatrix();
}

void LaserRenderable::setPosition(const Vec2& pos)
{
	this->pos = pos;
	updateMatrix();
}

void LaserRenderable::setAngle(float angle)
{
	this->angle = angle;
	updateMatrix();
}

void LaserRenderable::setLength(float length)
{
	laserLength = length;

}

void LaserRenderable::render(bool isIdentity, const Mat3& mat, const AARect& crop)
{
	cRenderableWithShader::render(isIdentity, mat, crop);
	shader->setWorldMatrix(isIdentity ? worldMatrix : worldMatrix * mat);
	laserTemplate->render(laserLength);
}

void LaserRenderable::updateMatrix()
{
	Mat3 mat = Mat3::rotationMatrix(angle).translateBy(pos);
	setWorldMatrix(mat);
}

void LaserTemplate::render(float laserLength)
{
	game->lastShader = nullptr;
	shader->begin();
	laserTexture->bindTexture();

	glBindBuffer(GL_ARRAY_BUFFER, laserQuad);
	glBindVertexArray(laserVertex);
	shader->bindAttribute(aYShift.index, 4 * 5, 4 * 0);
	shader->bindAttribute(widthMult1.index, 4 * 5, 4 * 1);
	shader->bindAttribute(widthMult2.index, 4 * 5, 4 * 2);
	shader->bindAttribute(widthMult3.index, 4 * 5, 4 * 3);
	shader->bindAttribute(xUV.index, 4 * 5, 4 * 4);


	shader->setUniform(beginX.index, laserBeginShift - laserBeginWidth);
	shader->setUniform(width1.index, laserBeginWidth);
	shader->setUniform(width2.index, laserLength + laserEndShift - laserBeginShift);
	shader->setUniform(width3.index, laserEndWidth);

	shader->setUniform(laserWidth.index, laserThickness);

	glDrawArrays(GL_TRIANGLES, 0, 12);
}

LaserTemplate::LaserTemplate(cGame *game, nlohmann::json& j, const DirentHelper::File& file)
{
	this->game = game;
	if (j.count("name"))
	{
		name = j["name"].get<std::string>();
	}

	laserThickness = j["laserThickness"].get<float>();

	laserBeginWidth = j["laserBeginWidth"].get<float>();
	laserEndWidth = j["laserEndWidth"].get<float>();

	laserBeginShift = j["laserBeginShift"].get<float>();
	laserEndShift = j["laserEndShift"].get<float>();

	laserTexture = resources.getTexture(file.folder + j["laserTexture"].get<std::string>());
	shader = resources.getShader(file.folder + j["laserShader"].get<std::string>());

	aYShift = shader->addAttribute("yShift", TypeFloat);
	widthMult1 = shader->addAttribute("widthMult1", TypeFloat);
	widthMult2 = shader->addAttribute("widthMult2", TypeFloat);
	widthMult3 = shader->addAttribute("widthMult3", TypeFloat);
	xUV = shader->addAttribute("xUV", TypeFloat);

	beginX = shader->addUniform("beginX", TypeFloat);
	width1 = shader->addUniform("width1", TypeFloat);
	width2 = shader->addUniform("width2", TypeFloat);
	width3 = shader->addUniform("width3", TypeFloat);
	laserWidth = shader->addUniform("laserWidth", TypeFloat);

	if (laserQuad == -1)
	{
		GLfloat vertexData[] =
		{
			-0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, 1.0f, 0.0f, 0.0f, 0.5f,
			0.5f, 1.0f, 0.0f, 0.0f, 0.5f,

			0.5f, 1.0f, 0.0f, 0.0f, 0.5f,
			0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
			-0.5f, 0.0f, 0.0f, 0.0f, 0.0f,

			-0.5f, 1.0f, 0.0f, 0.0f, 0.5f,
			-0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
			0.5f, 1.0f, 1.0f, 0.0f, 0.5f,

			0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
			0.5f, 1.0f, 0.0f, 0.0f, 0.5f,
			-0.5f, 1.0f, 0.0f, 0.0f, 0.5f,

			-0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
			-0.5f, 1.0f, 1.0f, 1.0f, 1.0f,
			0.5f, 1.0f, 1.0f, 1.0f, 1.0f,

			0.5f, 1.0f, 1.0f, 1.0f, 1.0f,
			0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
			-0.5f, 1.0f, 1.0f, 0.0f, 0.5f,
		};

		glGenBuffers(1, &laserQuad);
		glBindBuffer(GL_ARRAY_BUFFER, laserQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);


		glGenVertexArrays(1, &laserVertex);
		glBindVertexArray(laserVertex);
		glBindBuffer(GL_ARRAY_BUFFER, laserQuad);
	}
}

LaserTemplate::~LaserTemplate()
{
	shader = nullptr;
	laserTexture = nullptr;
}

const std::string& LaserTemplate::getName() const
{
	return name;
}
