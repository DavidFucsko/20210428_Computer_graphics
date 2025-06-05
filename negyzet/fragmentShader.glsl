#version 430

in vec3 vertexColor;
out vec4 color;

void main(void)
{
	color = vec4(vertexColor, 1.0);
}