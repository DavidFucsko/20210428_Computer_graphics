#version 430

void main(void)
{
	/** Pont kirajzol�sa. */
	//gl_Position = vec4(0.0, 0.0, 0.0, 1.0);

	/** 3 pont h�romsz�g kirajzol�s�hoz. */
	if(gl_VertexID == 0) gl_Position = vec4(0.25, -0.25, 0.0, 1.0);
	else if(gl_VertexID == 1) gl_Position = vec4(-0.25, -0.25, 0.0, 1.0);
	else gl_Position = vec4(0.0, 0.25, 0.0, 1.0);
}