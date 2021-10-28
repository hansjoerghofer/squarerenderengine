#version 450 core
 
out vec4 FragColor;

in vec2 TexCoords;
 
uniform sampler2D image;

uniform float threshold = 1;

uniform float intensity = 1;

uniform float maxBrightness = 10;

void main(void) 
{
    vec3 color = texture2D(image, TexCoords).rgb;

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    vec3 highVal = color * step(threshold, brightness);

    vec3 clamped = clamp(highVal, vec3(0), maxBrightness.xxx);

    FragColor = vec4(clamped * intensity, 1);
}