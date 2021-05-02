#version 330 core

#pragma include Includes/Test.glsl

out vec4 FragColor;
 
in vec4 vertexColor;

void main()
{
    FragColor = vertexColor;
}
