#version 430

uniform float offsetX;
uniform float offsetY;

out vec3 vertexColor;

void main(void)
{
    if (gl_VertexID == 0){
        gl_Position = vec4(-0.10 + offsetX, -0.10 + offsetY, 0.0, 1.0);
        vertexColor = vec3(1.0,0.0,0.0);
    }
    else if (gl_VertexID == 1){
         gl_Position = vec4( 0.10 + offsetX, -0.10 + offsetY, 0.0, 1.0);
         vertexColor = vec3(1.0,0.0,0.0);
    }
    else if (gl_VertexID == 2){
        gl_Position = vec4( 0.10 + offsetX,  0.10 + offsetY, 0.0, 1.0);
        vertexColor = vec3(1.0,0.0,0.0);
    }
    else if (gl_VertexID == 3){
        gl_Position = vec4( 0.10 + offsetX,  0.10 + offsetY, 0.0, 1.0);
        vertexColor = vec3(1.0,1.0,0.0);
    }
    else if (gl_VertexID == 4){
        gl_Position = vec4(-0.10 + offsetX,  0.10 + offsetY, 0.0, 1.0);
        vertexColor = vec3(1.0,1.0,0.0);
    }
    else{ // gl_VertexID == 5
        gl_Position = vec4(-0.10 + offsetX, -0.10 + offsetY, 0.0, 1.0);
        vertexColor = vec3(1.0,1.0,0.0);
    }
}