#version 110
attribute vec2 aVertexPosition;
attribute vec2 aVertexUV;
attribute vec4 aVertexColor;

uniform vec4 uColor;
uniform sampler2D uTexture0;
uniform mat3 uWorldMatrix;
uniform mat3 uViewMatrix;

varying vec4 vColor;
varying vec2 vVertexUV;

uniform float uScale;


void main(void) 
{
	vec3 worldPos = uWorldMatrix * vec3(aVertexPosition, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);

	vColor = uColor * aVertexColor;
	vVertexUV = aVertexUV;
}
