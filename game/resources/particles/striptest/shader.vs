#version 150
in vec2 pos;
in vec2 speed;
in float time;
in vec2 uv;

in vec2 prevpos;
in vec2 prevspeed;
in float prevtime;

uniform float uCurrentTime;
uniform mat3 uViewMatrix;

void main(void) 
{
	float dt = uCurrentTime - time;
	
    vec2 curPos = pos + dt * speed;
    vec2 prevPos = prevpos + (uCurrentTime - prevtime) * prevspeed;
    
    float rotation = atan( prevPos.y - curPos.y, prevPos.x - curPos.x);
    
	float s = sin(rotation);
	float c = cos(rotation);
    
	vec3 worldPos = vec3(curPos + vec2(0.0, uv.y - 0.5) * mat2(c, -s, s, c) * 30.0, 1.0);
	vec3 viewPos = uViewMatrix * worldPos;
	gl_Position = vec4(viewPos.x, viewPos.y, 0.0, 1.0);
}
