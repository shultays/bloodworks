#version 150
attribute vec2 pos;
attribute vec2 uv;
attribute float time;

attribute vec2 moveSpeed;
attribute vec2 uvStart;
attribute vec2 uvSize;

attribute vec2 uvShift;
attribute float color;

attribute float maxTime;
attribute float maxScale;

uniform float uCurrentTime;
uniform sampler2D uTexture0;
uniform mat3 uViewMatrix;

varying vec2 vVertexUV;
varying vec2 vUVMid;
varying vec2 vUVSize;

varying vec4 vColor;

void main(void) 
{
	float dt = ( uCurrentTime - time ) / maxTime;
    
    float s = maxScale / 200.0;
	float curScale = ( 30 + 150 * dt ) * s;
    
	vec3 worldPos = vec3(pos + moveSpeed * dt  * s+ (uv * 2.0 - vec2(1.0, 1.0)) * curScale, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
	
    
    vec2 uvStart2 = uvStart + uvShift * dt;
    
    vVertexUV = uvStart2 + uvSize * uv;
    vUVMid = uvStart2 + uvSize * 0.5;
    vUVSize = uvSize;
    
    float alpha = 0.0;
    if ( dt < 0.25){
        alpha = dt / 0.25;
    } else if ( dt < 0.9) {
        alpha = 1.0;
    } else if ( dt < 1.2) {
        alpha  = (1.2 - dt ) / 0.3;
    }
    
    const vec3 c0 = vec3(1.0, 0.737, 0.302);
    const vec3 c1 = vec3(0.996, 0.365, 0.063);
    
    vColor = vec4( c0 * color + c1 * (1.0 - color), alpha  );
}
