#version 430 compatibility

uniform vec2 offset;
uniform float size;
out vec4 myColor;

void main(void)
{
	vec4 pos = gl_Vertex;
	pos.xy += offset;
	gl_Position = pos;
	gl_PointSize = size;
	myColor = gl_Color;
}