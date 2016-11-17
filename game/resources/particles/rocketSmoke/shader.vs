attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute float color;
attribute vec2 moveSpeed;
attribute float initialScale;
attribute float scaleSpeed;
attribute float initialRotation;
attribute float rotateSpeed;
attribute float initialAlpha;
attribute float fadeOutSpeed;

uniform float currentTime;
uniform sampler2D uTexture;

varying vec4 vColor;
varying vec2 vVertexUV;

uniform mat3 uViewMatrix;


void main(void) 
{
	float dt = currentTime - time;
	
	float r = initialRotation + rotateSpeed * dt;
	float cr = cos(r);
	float sr = sin(r);
	mat2 rotationMatrix = mat2( cr, -sr,
                                sr, cr);
							   
	float curScale = initialScale + scaleSpeed * dt;
	vec3 worldPos = vec3(pos + moveSpeed * dt + (rotationMatrix * (uv * 2.0 - vec2(1.0, 1.0))) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vColor = vec4(color, color, color, initialAlpha - fadeOutSpeed * dt);
	
	vVertexUV = uv;
}
