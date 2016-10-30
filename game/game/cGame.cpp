#include "cGame.h"
#include "cTools.h"
#include "cGlobals.h"
#include "stdio.h"
#include "cResources.h"

GLuint gVBO;

cTextureShr texture;
cShaderShr s;

cTextureShr dogs[10];
cTextureShr kicks[10];

Vec2 dogShifts[10];
Vec2 kickShifts[10];

struct Dog
{
	Vec2 pos;
	Vec2 target;
	int type;

	Vec2 kickPos;
	int kickType;

	float angle;
	float kickAngle;
};


Dog ds[5];

Vec2 spawn[5];
int cur = 0;

float randFloat()
{
	return (rand() % 10000) / 10000.0f;
}

float randFloat(float min, float max)
{
	return randFloat() * (max - min) + min;
}

void resetDog(int i)
{
	ds[i].target = spawn[i] + Vec2(randFloat(-20, 20), randFloat(-20, 20));
	bool same = true;
	while(same)
	{
		ds[i].type = rand() % 10;
		same = false;
		for (int j = 0; j < 5; j++)
		{
			if (j == i)
			{
				continue;
			}
			if (ds[i].type == ds[j].type)
			{
				same = true;
				break;
			}
		}
	}
	ds[i].pos = ds[i].target - Vec2(400, 300);
	ds[i].pos.normalize();
	ds[i].pos *= 500;
	ds[i].pos += Vec2(400, 300);

	ds[i].kickPos = spawn[i];
	ds[i].kickType = -1;

	ds[i].angle = randFloat();
}


void cGame::init()
{
	s = resources.getShader("resources/default.vs", "resources/default.ps");

	//VBO data
	GLfloat vertexData[] =
	{
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	};


	//IBO data
	GLuint indexData[] = { 0, 1, 2, 3 };

	//Create VBO
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	texture = resources.getTexture("dogs/bg.png");


	for (int i = 0; i < 10; i++)
	{
		std::string str = "dogs/d";
		str += i + '0';
		str += ".png";
		dogs[i] = resources.getTexture(str.c_str());

		str = "dogs/k";
		str += i + '0';
		str += ".png";
		kicks[i] = resources.getTexture(str.c_str());
		dogShifts[i].setZero();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	dogShifts[0].x = 0.0f;
	dogShifts[0].y = -37.5f;

	dogShifts[1].x = 15.0f;
	dogShifts[1].y = -22.5f;

	dogShifts[2].x = -33.0f;
	dogShifts[2].y = -16.5f;

	dogShifts[3].x = 0.0f;
	dogShifts[3].y = -7.5f;

	dogShifts[4].x = -18.0f;
	dogShifts[4].y = -37.5f;

	dogShifts[5].x = 0.0f;
	dogShifts[5].y = -22.5f;

	dogShifts[6].x = 18.0f;
	dogShifts[6].y = 0.0f;

	dogShifts[7].x = -22.5f;
	dogShifts[7].y = -22.5f;

	dogShifts[8].x = 27.0f;
	dogShifts[8].y = -22.5f;

	dogShifts[9].x = -19.5f;
	dogShifts[9].y = -25.5f;


	kickShifts[0].x = 23;
	kickShifts[0].y = -3;
	kickShifts[1].x = 26;
	kickShifts[1].y = -14;


	kickShifts[2].x = -46.0f;
	kickShifts[2].y = -0.0f;

	kickShifts[3].x = -26.0f;
	kickShifts[3].y = -7.0f;
	kickShifts[4].x = -41.0f;
	kickShifts[4].y = -25.0f;
	kickShifts[5].x = 23.0f;
	kickShifts[5].y = -23.0f;
	kickShifts[6].x = 36.0f;
	kickShifts[6].y = 17.0f;
	kickShifts[7].x = 45.0f;
	kickShifts[7].y = 34.0f;
	kickShifts[8].x = 44.0f;
	kickShifts[8].y = -10.0f;

	kickShifts[9].x = -44.0f;
	kickShifts[9].y = -27.0f;
	cur = 0;


	spawn[0] = Vec2(200, 150);
	spawn[1] = Vec2(600, 150);


	spawn[4] = Vec2(400, 300);


	spawn[2] = Vec2(600, 450);
	spawn[3] = Vec2(200, 450);


	for (int i = 0; i < 5; i++)
	{
		resetDog(i);
	}
}


void cGame::tick(float dt)
{
	Vec3 v;
	v.setZero();
	v.z = 1.0f;
	Mat3 m;
	m.makeIdentity();
	m.scaleBy(10);
	m.translateBy(10, 20);
	Vec3 v2 = v * m;


	bool b = false;
	for (int i = 0; i < 5; i++)
	{
		if (ds[i].pos.distance(ds[i].target) > 2.0f)
		{
			Vec2 dir = ds[i].target - ds[i].pos;
			dir.normalize();
			ds[i].pos += dir * dt * 250.0f;

			ds[i].angle += dt * 10.0f;
			if (ds[i].angle > pi_2)
			{
				ds[i].angle -= pi_2;
			}
		}
		else
		{
			if (ds[i].angle != 0.0f)
			{
				ds[i].angle += dt * 10.0f;
				if (ds[i].angle > pi_2)
				{
					ds[i].angle = 0.0f;
				}
			}
		}

		if (ds[i].kickType == -1)
		{
			if (b == false && input.isMousePressed(SDL_BUTTON_LEFT) && ds[i].pos.distance(Vec2(input.x, input.y)) < 100.0f)
			{
				b = true;
				ds[i].kickType = cur;
				ds[i].kickPos = ds[i].pos;
				cur++;
				if (cur > 9) cur = 0;
				int t = ds[i].kickType;
				if (t == 0 || t == 1 || t == 5 || t == 6 || t == 7 || t == 8)
				{
					ds[i].kickPos.x -= 500;
				}
				else
				{

					ds[i].kickPos.x += 500;
				}

				ds[i].kickPos.y -= randFloat(-20, 20);
			}
		}
		else
		{
			if (ds[i].kickPos.distance(ds[i].pos) > 15.0f)
			{
				Vec2 dir = ds[i].pos - ds[i].kickPos;
				dir.normalize();
				ds[i].kickPos += dir * dt * 1200.0f;
			}
			else
			{
				resetDog(i);
			}
		}
	}

}

void cGame::render()
{
	s->begin();


	//Set vertex data
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);

	s->bindPosition(sizeof(float) * 8, 0);
	s->bindUV(sizeof(float) * 8, sizeof(float) * 2);
	s->bindColor(sizeof(float) * 8, sizeof(float) * 4);

	s->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	float left = 0;
	float right = 800;
	float bottom = 0;
	float top = 600;


	Mat3 viewMatrix = Mat3::identity();

	viewMatrix.translateBy(-left, -top).scaleBy(1.0f/(right-left), 1.0f/(bottom - top)).translateBy(-0.5f).scaleBy(2.0f);

	s->setViewMatrix(viewMatrix);

	texture->bindTexture();
	s->setWorldMatrix(Mat3::scaleMatrix(400, 300).translateBy(400, 300));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	for (int i = 0; i < 5; i++)
	{
		dogs[ds[i].type]->bindTexture();
		s->setWorldMatrix(Mat3::identity()
			.rotateBy(pi * 0.1f * sin(ds[i].angle))
			.scaleBy(dogs[ds[i].type]->getDimensions().toVec() * 0.3f)
			.translateBy(ds[i].pos - dogShifts[ds[i].type])
		);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	for (int i = 0; i < 5; i++)
	{
		if (ds[i].kickType != -1)
		{
			kicks[ds[i].kickType]->bindTexture();
			s->setWorldMatrix(
				Mat3::identity()
				.scaleBy(kicks[ds[i].kickType]->getDimensions().toVec() * 0.5f)
				.translateBy(ds[i].kickPos - kickShifts[ds[i].kickType] * 2.5f));
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
	}
	
	glDisableVertexAttribArray(0);

	glDisable(GL_TEXTURE_2D);

	//Unbind program
	glUseProgram(NULL);
}
