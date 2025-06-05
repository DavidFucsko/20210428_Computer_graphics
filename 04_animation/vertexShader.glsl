#version 430

uniform float offsetX;
uniform float offsetY;

void main(void)
{
	if(gl_VertexID == 0) gl_Position = vec4(0.25 + offsetX, -0.25 + offsetY, 0.0, 1.0);
	else if(gl_VertexID == 1) gl_Position = vec4(-0.25 + offsetX, -0.25 + offsetY, 0.0, 1.0);
	else gl_Position = vec4(0.0 + offsetX, 0.25 + offsetY, 0.0, 1.0);
}