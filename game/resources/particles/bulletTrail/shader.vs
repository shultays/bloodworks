#version 150
in vec2 pos;
in vec2 uv;
in float time;
in float startFadeinSpeed;

in vec3 color;
in float initialScale;
in float scaleSpeed;
in float initialAlpha;
in float fadeOutSpeed;
in float rotation;

uniform float uCurrentTime;
uniform sampler2D uTexture0;
uniform mat3 uViewMatrix;

uniform float lastBulletTime;

out vec4 vColor;
out vec2 vVertexUV;

void main(void) 
{
	float dt = uCurrentTime - time;
	dt = min(dt, 10.0);
	
	float curScale = initialScale + dt * scaleSpeed;
	
	float s = sin(rotation);
	float c = cos(rotation);
	vec3 worldPos = vec3(pos + vec2(uv.x, uv.y - 0.5) * mat2(c, -s, s, c) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
    float a = initialAlpha;
    
	a *= min((lastBulletTime - time) * 220.0, 1.0);
	vColor = vec4(mix(vec3(0.6), color,  min((lastBulletTime - time) * 10.0, 1.0)), min(a - fadeOutSpeed * dt, a * min(time * startFadeinSpeed, 1.0)));
	vVertexUV = uv;
}
