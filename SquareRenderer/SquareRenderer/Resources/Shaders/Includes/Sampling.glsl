//? #version 450 core

//-----------------------------------------------
//            SAMPLING FUNCTIONS
//-----------------------------------------------

float _radicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 _hammersley(uint i, uint N)
{
    return vec2(float(i) / float(N), _radicalInverse_VdC(i));
}

vec2 _hammersleyOpt(uint i, float invN)
{
    return vec2(float(i) * invN, _radicalInverse_VdC(i));
}

// vec3 _hemisphereSample_uniform(float u, float v) 
// {
//     float phi = v * 2.0 * PI;
//     float cosTheta = 1.0 - u;
//     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
//     return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
// }
//    
// vec3 _hemisphereSample_cos(float u, float v) 
// {
//     float phi = v * 2.0 * PI;
//     float cosTheta = sqrt(1.0 - u);
//     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
//     return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
// }