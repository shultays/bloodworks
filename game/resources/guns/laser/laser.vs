#version 150
attribute float yShift;
attribute float widthMult1;
attribute float widthMult2;
attribute float widthMult3;
attribute float xUV;

uniform sampler2D uTexture0;
uniform mat3 uWorldMatrix;
uniform mat3 uViewMatrix;

uniform float beginX;
uniform float width1;
uniform float width2;
uniform float width3;
uniform float laserWidth;

uniform float maxRange;

varying vec4 vColor;
varying vec2 vVertexUV;
varying float curLength;

void main(void) 
{
	curLength = width1 * widthMult1 + width2 * widthMult2 + width3 * widthMult3;
	vec3 worldPos = uWorldMatrix * vec3(beginX + curLength, laserWidth * yShift, 1.0);
	
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);

	vColor = vec4(1.0f);
	vVertexUV = vec2(xUV, yShift + 0.5);
}
