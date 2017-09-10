#version 110
attribute vec2 aVertexUV;

uniform sampler2D uTexture0;

varying vec2 vScreenUV;

void main(void) 
{
	gl_Position = vec4(aVertexUV, 0.0, 1.0);
	vScreenUV = aVertexUV * 0.5 + 0.5;
}
