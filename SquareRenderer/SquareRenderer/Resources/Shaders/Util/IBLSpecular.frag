#version 330 core

#pragma include ../Includes/PBR.glsl //! #include "../Includes/PBR.glsl"

in vec4 FragPos;
out vec4 Irradiance;

uniform float resolution;
uniform float roughness;
uniform samplerCube hdri;

void main()
{		
    vec3 N = normalize(FragPos.xyz);    
    vec3 R = N;
    vec3 V = R;

     float res = resolution * 2; // unclear why needed

    const uint SAMPLE_COUNT = 2048u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);     
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = _hammersley(i, SAMPLE_COUNT);
        vec3 H  = _importanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            // start reduce sample artifacts
            float D         = _distributionGGX(N, H, roughness);
            float NdotH     = max(dot(N, H), 0.0);
            float HdotV     = max(dot(H, V), 0.0);
            float pdf       = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float saTexel   = 4.0 * PI / (6.0 * res * res);
            float saSample  = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.00001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            prefilteredColor += textureLod(hdri, L, mipLevel).rgb * NdotL;
            //end reduce sample artifacts

            //prefilteredColor += texture(environmentMap, L).rgb * NdotL;
            
            totalWeight      += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / max(totalWeight, 0.001f);

    Irradiance = vec4(prefilteredColor, 1.0);
}  