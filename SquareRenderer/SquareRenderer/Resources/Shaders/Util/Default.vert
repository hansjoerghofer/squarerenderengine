#version 450 core

//-----------------------------------------------
// DEFAULT VERTEX SHADER: POSITION PASS THROUGH
//-----------------------------------------------

layout (location = 0) in vec3 vPosition;

void main()
{
    gl_Position =  vec4(vPosition, 1.0);
}