attribute vec2 pos;
attribute vec2 uv;
attribute float time;
attribute float startFadeinSpeed;

attribute vec3 color;
attribute float initialScale;
attribute float scaleSpeed;
attribute float initialAlpha;
attribute float fadeOutSpeed;
attribute float rotation;

uniform float uCurrentTime;
uniform sampler2D uTexture0;
uniform mat3 uViewMatrix;

uniform float lastBulletTime;

varying vec4 vColor;
varying vec2 vVertexUV;

void main(void) 
{
	float dt = uCurrentTime - time;
	
	float curScale = initialScale + dt * scaleSpeed;
	
	float s = sin(rotation);
	float c = cos(rotation);
	vec3 worldPos = vec3(pos + vec2(uv.x, uv.y - 0.5) * mat2(c, -s, s, c) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	initialAlpha *= min(time * startFadeinSpeed, 1.0);
	initialAlpha *= min((lastBulletTime - time) * 120.0, 1.0);
	vColor = vec4(lerp(vec3(0.6), color,  min((lastBulletTime - time) * 10.0, 1.0)), initialAlpha - fadeOutSpeed * dt);
	vVertexUV = uv;
}
