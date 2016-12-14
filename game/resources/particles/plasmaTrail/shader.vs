attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute float color;
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
	
	float curScale = initialScale + scaleSpeed * dt;
	vec3 worldPos = vec3(pos + (uv * 2.0 - vec2(1.0, 1.0)) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vec3 finalColor = vec3(color);
	vColor = vec4(finalColor, initialAlpha - fadeOutSpeed * dt);
	
	vVertexUV = uv;
}
