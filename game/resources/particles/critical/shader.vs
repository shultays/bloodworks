#version 110
attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute vec3 color;
attribute vec2 moveSpeed;
attribute float initialScale;
attribute float rotateSpeed;
attribute vec2 uvStart;

uniform float uCurrentTime;
uniform sampler2D uTexture0;
uniform mat3 uViewMatrix;

varying vec4 vColor;
varying vec2 vVertexUV;


void main(void) 
{
	float dt = uCurrentTime - time;
	dt = min(dt, 10.0);
	
	float curScale = initialScale - dt * 32.0;
	curScale = max(curScale, 0.0);
	float rotate = rotateSpeed * (3.1415 + dt);
	float s = sin(rotate);
	float c = cos(rotate);
	vec3 worldPos = vec3(pos + moveSpeed * dt + (uv - 0.5) * mat2(c, -s, s, c) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vec3 finalColor = color;
	
	
	vColor = vec4(finalColor, 0.2 - dt * 2.0  + sin(dt * 12.0) * 1.0);
	
	vVertexUV = uv * 0.25 + uvStart;
}
