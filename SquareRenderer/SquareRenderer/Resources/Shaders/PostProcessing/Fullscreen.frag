#version 450 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D image;

uniform float level = 0;

//uniform vec4 scale = vec4(1,1,1,1);

//uniform vec4 shift = vec4(0,0,0,0);

void main()
{
    //FragColor = scale * textureLod(image, TexCoords, level) + shift;
    FragColor = textureLod(image, TexCoords, level);
}