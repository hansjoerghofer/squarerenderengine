#version 330 core

#pragma include ../Includes/Globals.glsl //! #include "../Includes/Globals.glsl"
#pragma include ../Includes/Sampling.glsl //! #include "../Includes/Sampling.glsl"
#pragma include ../Includes/PBR.glsl //! #include "../Includes/PBR.glsl"

in vec2 TexCoords;
out vec2 FragColor;

vec2 IntegrateBRDF(float NdotV, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = _hammersley(i, SAMPLE_COUNT);
        vec3 H  = _importanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = clamp(L.z, 0.0, 1.0);
        float NdotH = clamp(H.z, 0.0, 1.0);
        float VdotH = clamp(dot(V, H), 0.0, 1.0);

        if(NdotL > 0.0)
        {
            float G = _geometrySmith_IBL(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}
// ----------------------------------------------------------------------------

void main() 
{
    FragColor = IntegrateBRDF(TexCoords.x, TexCoords.y);
}