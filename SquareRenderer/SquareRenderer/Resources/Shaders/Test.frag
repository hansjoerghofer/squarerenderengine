#version 330 core

#pragma include Includes/Test.glsl //! #include "Includes/Test.glsl"

out vec4 FragColor;
 
in vec3 vertexColor;

void main()
{
    FragColor = vec4(vertexColor, 1);
}
