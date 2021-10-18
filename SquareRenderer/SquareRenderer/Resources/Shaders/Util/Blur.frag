#version 450 core
 
out vec4 FragColor;

in vec2 TexCoords;
 
uniform sampler2D image;

uniform vec4 dim;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main(void) 
{
    FragColor = texture2D(image, TexCoords) * weight[0];

    for (int i=1; i<3; i++)
    {
        #if FILTER_VERTICAL
            vec2 offset = vec2(offset[i], 0) * dim.zw;
        #else
            vec2 offset = vec2(0, offset[i]) * dim.zw;
        #endif

        FragColor += texture2D(image, TexCoords + offset) * weight[i];
        FragColor += texture2D(image, TexCoords - offset) * weight[i];
    }
}