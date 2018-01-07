#version 150
in vec2 pos;
in vec2 uv;
in float time;

in vec3 color;
in float initialScale;
in float scaleSpeed;
in float initialAlpha;
in float fadeOutSpeed;

uniform float uCurrentTime;
uniform sampler2D uTexture0;
uniform mat3 uViewMatrix;

out vec4 vColor;
out vec2 vVertexUV;


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
