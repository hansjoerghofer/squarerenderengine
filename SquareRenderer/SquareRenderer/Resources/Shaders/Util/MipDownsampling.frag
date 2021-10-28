#version 450 core
 
out vec4 FragColor;

in vec2 TexCoords;
 
uniform sampler2D image;

uniform float level = 0;

void main(void) 
{
    FragColor = textureLod(image, TexCoords, level);
}