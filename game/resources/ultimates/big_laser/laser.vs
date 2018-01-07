#version 150
in float yShift;
in float widthMult1;
in float widthMult2;
in float widthMult3;
in float xUV;

uniform sampler2D uTexture0;
uniform mat3 uWorldMatrix;
uniform mat3 uViewMatrix;

uniform float beginX;
uniform float width1;
uniform float width2;
uniform float width3;
uniform float laserWidth;

uniform float maxRange;
uniform float time;

out vec4 vColor;
out vec2 vVertexUV;
out float curLength;

out float xPos;

void main(void) 
{
	curLength = width1 * widthMult1 + width2 * widthMult2 + width3 * widthMult3;
	vec3 worldPos = uWorldMatrix * vec3(beginX + curLength, laserWidth * yShift, 1.0);
	
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);

	vColor = vec4(1.0);
	vVertexUV = vec2(xUV, yShift + 0.5);
	
	xPos = xUV;
}
