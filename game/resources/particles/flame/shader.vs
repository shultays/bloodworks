#version 110
attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute vec3 color;
attribute vec2 moveSpeed;
attribute float initialScale;
attribute float rotateSpeed;
attribute float lifeTime;
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
	
	float curScale = initialScale + dt * 60.0;
	curScale = min(curScale, 30.0);
	
	float rotate = rotateSpeed * (3.1415 + dt);
	float s = sin(rotate);
	float c = cos(rotate);
	vec3 worldPos = vec3(pos + moveSpeed * dt + (uv - 0.5) * mat2(c, -s, s, c) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vec3 finalColor = color;
	finalColor.g *= 1.0 - dt / 1.0; 
	finalColor.b *= 1.0 - dt / 1.0; 
	
	float alpha = 0.4;
	if (dt > 0.3){
		alpha -= dt - 0.3;
	}
	
	float alphaMult = 1.0 - (dt - lifeTime) * 20.0;
	if (alphaMult < 0.0){
		alpha = 0.0;
	} else if (alphaMult < 1.0){
		alpha *= alphaMult;
	}
	vColor = vec4(finalColor, alpha);
	
	vVertexUV = uv * vec2(1.0/12.0, 1.0/6.0) + uvStart;
}
