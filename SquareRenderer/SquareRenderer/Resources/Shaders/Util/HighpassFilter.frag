#version 450 core
 
out vec4 FragColor;

in vec2 TexCoords;
 
uniform sampler2D image;

uniform float threshold = 1;

void main(void) 
{
    vec3 color = texture2D(image, TexCoords).rgb;
    FragColor = vec4(color * step(threshold, color), 1);
}