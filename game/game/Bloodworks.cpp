#include "Bloodworks.h"
#include "cRenderable.h"

cTexturedQuadRenderable *q;
void Bloodworks::init()
{
	bg = new cTexturedQuadRenderable(this, "resources/bg.png", "resources/default");
	bg->setSize(512, 512);
	addRenderable(bg);

	body = new cTexturedQuadRenderable(this, "resources/assault/body.png", "resources/default");
	body->setSize(15.7f, 22.9f);
	addRenderable(body);

	hands = new cTexturedQuadRenderable(this, "resources/assault/hands.png", "resources/default");
	hands->setSize(15.7f, 22.9f);
	addRenderable(hands);

	pos = Vec2(100, 100);
	angle = pi_d2;
	speed = 0.0f;

	q = new cTexturedQuadRenderable(this, "resources/s.png", "resources/default");
	q->setSize(1);
	addRenderable(q);



}

void Bloodworks::tick(float dt)
{
	Mat3 mat = Mat3::identity();
	mat.scaleBy(15.7f, 22.9f);
	mat.translateBy(0.0f, 5.0f);
	mat.rotateBy(angle);
	mat.translateBy(pos);

	body->setWorldFrame(mat);
	hands->setWorldFrame(mat);

	q->setPosition(pos);
}

