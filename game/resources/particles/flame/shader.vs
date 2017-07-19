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
	
	vColor = vec4(finalColor, 0.4 - dt * 0.5 * (2));
	
	vVertexUV = uv * vec2(1.0/12.0, 1.0/6.0) + uvStart;
}
