#pragma once

#include <GL/glew.h>

const float update_interval = 0.01f;
const float draw_interval = 1.0f / 60.0f;

extern GLuint postProcessQuad;

class Coral 
{
	friend class cGame;

	float lastDrawTime;
	float lastUpdateTime;
	bool tickedBeforeRender;

	GLuint tempFrameBuffer[2];
	GLuint tempFrameBufferTexture[2];
public:

	Coral();
	void init();
	void tick();
};
