#version 450 core
 
out vec4 FragColor;

in vec2 TexCoords;
 
uniform sampler2D image;

uniform float threshold = 1;

void main(void) 
{
    vec3 color = texture2D(image, TexCoords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    FragColor = vec4(color * step(threshold, brightness), 1);
}