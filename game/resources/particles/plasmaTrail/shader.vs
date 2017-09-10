#version 110
attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute vec3 color;
attribute float initialScale;
attribute float scaleSpeed;
attribute float initialAlpha;
attribute float fadeOutSpeed;

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
	vec3 worldPos = vec3(pos + (uv * 2.0 - vec2(1.0, 1.0)) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vColor = vec4(color, initialAlpha - fadeOutSpeed * dt);
	
	vVertexUV = uv;
}
