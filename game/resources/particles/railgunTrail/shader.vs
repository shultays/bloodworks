#version 110
attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute float color;
attribute vec2 moveSpeed;
attribute float initialScale;
attribute float scaleSpeed;
attribute float initialAlpha;
attribute float fadeOutSpeed;
attribute float rotation;

uniform float uCurrentTime;
uniform sampler2D uTexture0;
uniform mat3 uViewMatrix;

varying vec4 vColor;
varying vec2 vVertexUV;


void main(void) 
{
	float dt = uCurrentTime - time;
	dt = min(dt, 10.0);
	
	float curScale = initialScale + scaleSpeed * dt;
	curScale = min(curScale, 8.0);
	
	float s = sin(rotation);
	float c = cos(rotation);
	
	vec3 worldPos = vec3(pos + moveSpeed * dt + (uv * 2.0 - vec2(1.0, 1.0)) * curScale * mat2(c, -s, s, c), 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vec3 finalColor = vec3(color);
	float alpha = initialAlpha - fadeOutSpeed * dt;
	alpha = min(alpha, 0.7);
	vColor = vec4(finalColor, alpha);
	
	vVertexUV = uv;
}
