#version 450 core

#pragma include ../Includes/Globals.glsl //! #include "../Includes/Globals.glsl"
#pragma include ../Includes/Common.glsl //! #include "../Includes/Common.glsl"
#pragma include ../Includes/Lights.glsl //! #include "../Includes/Lights.glsl"
#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"
#pragma include ../Includes/Sampling.glsl //! #include "../Includes/Sampling.glsl"
#pragma include ../Includes/Shadow.glsl //! #include "../Includes/Shadow.glsl"
#pragma include ../Includes/PBR.glsl //! #include "../Includes/PBR.glsl"

in VertexShaderData
{
    vec2 uv;
    vec3 normalWS;
    vec3 fragmentPosWS;
    
    mat3 TBN;

    vec4[_MAX_SIZE_LIGHT] pFragmentLS;
} IN;

layout (location = 0) out vec4 OutputShaded;
//layout (location = 0) out vec4 OutputShadedRoughness;
//layout (location = 1) out vec4 OutputNormalDepth;

// uniforms
uniform sampler2D albedoTexture; // [white]
uniform sampler2D pbrAttributesTexture; // [white]
uniform sampler2D normalMap; // [normal]

uniform vec4 albedoColor = vec4(1,1,1,1);
uniform float metallicFactor = 1; // [0, 1]
uniform float roughnessFactor = 1; // [0, 1]
uniform float aoFactor = 1; // [0, 1]

uniform sampler2D   brdfLUT;
uniform samplerCube prefilterMap;
uniform samplerCube irradianceMap;

struct Material
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

vec3 sampleNormal()
{
    vec3 normal = texture(normalMap, IN.uv).xyz;
    normal = normalize(normal * 2.0 - 1.0);   
    return normalize(IN.TBN * normal);
    //return normalize(IN.normal);
}

vec3 IBL(in vec3 N, in vec3 V, in vec3 R, in vec3 F0, in Material mat)
{
    vec3 F = _fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, mat.roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - mat.metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * mat.albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor   = textureLod(prefilterMap, R,  mat.roughness * MAX_REFLECTION_LOD).rgb;   
    vec2 envBRDF            = texture(brdfLUT, vec2(max(dot(N, V), 0.0), mat.roughness)).rg;
    vec3 specular           = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    
    //float visibility = 0.5 + _shadow(IN.pFragmentLS[0], 0) * 0.5;

    return mat.ao * (kD * diffuse + specular);// * visibility;
}

void main() 
{
#if DISABLE_ALBEDO_TEXTURE
    vec4 cAlbedo = albedoColor;
#else
    vec4 cAlbedo = albedoColor * texture(albedoTexture, IN.uv);
#endif

#if ALPHA_CUTOUT
    // cutout alpha
    if(cAlbedo.a < 0.001) discard;
#endif

    vec3 pCamera = _cameraPosition();
    
    Material mat;
    mat.albedo = cAlbedo.rgb;

    vec4 pbrAttr = texture(pbrAttributesTexture, IN.uv);
    mat.metallic = metallicFactor * pbrAttr.r;
    mat.roughness = roughnessFactor * pbrAttr.g;
    mat.ao = aoFactor * pbrAttr.b;

    vec3 P = IN.fragmentPosWS;
    vec3 N = sampleNormal();
    vec3 V = normalize(pCamera - P);
    vec3 R = normalize(reflect(-V, N));

    vec3 L;
    vec3 cLight = vec3(0.0);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, mat.albedo, mat.metallic);
    
    // process point lights
    for(int i = 0; i < _numLights; ++i)
    {
        float attenuation = 1.0;

        // point light
        if(_lightsPosWS[i].w > 0)
        {
            vec3 LP = _lightsPosWS[i].xyz - IN.fragmentPosWS;
            attenuation = 10000.0 / dot(LP, LP);

            L = normalize(LP);
        }
        // directional light
        else
        {
            L = -normalize(_lightsPosWS[i].xyz);
        }

        vec3 radiance = _lightsColor[i].rgb * attenuation; 
        vec3 H = normalize(V + L);

        float NDF = _distributionGGX(N, H, mat.roughness);       
        float G   = _geometrySmith(N, V, L, mat.roughness); 
        vec3 F    = _fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - mat.metallic;

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular     = numerator / max(denominator, 0.001);

        // shadow calculation
        float visibility = _shadow(IN.pFragmentLS[i], i);

        float NdotL = max(dot(N, L), 0.0);        
        cLight += (kD * mat.albedo / PI + specular) * radiance * NdotL * visibility;
    }

    vec3 cAmbient = IBL(N, V, R, F0, mat) * mat.ao;

    vec3 cShaded = cLight + cAmbient;

    //OutputShadedRoughness = vec4(cShaded, mat.roughness);
    OutputShaded = vec4(cShaded, cAlbedo.a);
    //OutputNormalDepth = vec4(N.xyz, gl_FragCoord.z);
    //OutputNormalDepth = vec4(P.xyz, gl_FragCoord.z);
}