#version 450 core
 
out vec4 FragColor;

in vec2 TexCoords;
 
uniform sampler2D image;

uniform vec4 dim;

#if FILTER_KERNEL_LDR_9x9

const int samples = 3; 
uniform float offset[samples] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[samples] = float[](0.2270270270, 0.3162162162, 0.0702702703);

#else //FILTER_KERNEL_HDR_33x33

const int samples = 9; 
uniform float offset[samples] = float[](
    0, 
    1.4545454545454546, 3.393939393939394, 5.333333333333333, 7.2727272727272725, 
    9.212121212121213, 11.151515151515152, 13.090909090909092, 15.030303030303031);
uniform float weight[samples] = float[](
    0.13994993409141898, 
    0.24148223921656609, 0.13345071114599705, 0.04506127908825874, 0.008979602716863155, 
    0.0009946636855602264, 5.52590936422348e-05, 1.2703239917755127e-06, 7.683411240577698e-09);

#endif

void main(void) 
{
    FragColor = texture2D(image, TexCoords) * weight[0];

    for (int i=1; i<samples; i++)
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