#version 330 core

out vec4 FragColor;
 
uniform vec4 albedoColor = vec4(1,1,1,1);

void main()
{
    FragColor = albedoColor;
}
