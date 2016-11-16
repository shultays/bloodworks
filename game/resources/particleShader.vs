attribute vec2 pos;
attribute vec2 initialScale;
attribute vec2 uv;
attribute vec4 color;
attribute vec2 moveSpeed;
attribute vec2 scaleSpeed;
attribute float time;
attribute float rotate;
attribute float rotateSpeed;
attribute float fadeOutSpeed;

uniform float currentTime;
uniform sampler2D uTexture;

varying vec4 vColor;
varying vec2 vVertexUV;

uniform mat3 uViewMatrix;


void main(void) 
{
	vec3 worldPos = vec3(pos + (uv * 2.0 - vec2(1.0, 1.0)) * initialScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vColor = vec4(1.0);
	vVertexUV = vec2(0.0);
	vColor.r  = uv.x;
	vColor.g  = uv.y;
	vVertexUV.x = pos.x + initialScale.x + uv.x + color.x + moveSpeed.x + scaleSpeed.x + time + rotate + rotateSpeed + fadeOutSpeed + currentTime;
}
