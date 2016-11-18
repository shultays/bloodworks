attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute vec3 color;
attribute vec2 moveSpeed;
attribute float initialScale;
attribute float scaleSpeed;
attribute float initialAlpha;
attribute float fadeOutSpeed;
attribute vec2 uvStart;
attribute vec2 uvSize;

uniform float currentTime;
uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform mat3 uViewMatrix;

varying vec4 vColor;
varying vec2 vVertexUV;
varying vec2 vPos;
varying vec2 vVertexPos;
varying float vMaxDist;

void main(void) 
{
	float dt = currentTime - time;
	
	float curScale = initialScale + scaleSpeed * dt;
	vec3 worldPos = vec3(pos + moveSpeed * dt + (uv * 2.0 - vec2(1.0, 1.0)) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
	vec3 finalColor = vec3(color);
	vColor.rgb = finalColor;
	
	dt /= fadeOutSpeed;
	if (dt < 0.1f)
	{
		vColor.a = dt / 0.1;
	}
	else if (dt > 1.0)
	{
		vColor.a = 1.0 - (dt - 1.0) / 1.5;
	}
	else
	{
		vColor.a = 1.0f;
	}
	
	vColor.a *= 0.3;
	
	vVertexUV = uv * uvSize + uvStart;
	vPos = pos + moveSpeed * dt;
	vVertexPos = worldPos.xy;
	vMaxDist = curScale;
}
