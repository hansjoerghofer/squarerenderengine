#version 330 core

#pragma include Includes/Lights.glsl //! #include "Includes/Lights.glsl"

out vec4 FragColor;
 
in EvaluationShaderData
{
    vec2 uv;
    vec3 normal;
    vec3 pFragmentWS;
    
    mat3 TBN;

    vec4[_MAX_SIZE_LIGHT] pFragmentLS;
} IN;

layout (location = 0) out vec4 OutputShadedRoughness;
//layout (location = 1) out vec4 OutputNormalDepth;

// uniforms
uniform sampler2D albedoTexture; // [white]
uniform sampler2D pbrAttributesTexture; // [white]
uniform sampler2D normalMap; // [normal]

uniform vec4 albedoColor = vec4(1,1,1,1);
uniform float metallicFactor = 1; // [0, 1]
uniform float roughnessFactor = 1; // [0, 1]
uniform float aoFactor = 1; // [0, 1]

void main()
{
    FragColor = vec4(IN.normal * 0.5 + 0.5, 1);
    //FragColor = vec4(IN.uv, 0, 1);

    //FragColor = texture(albedoTexture, IN.uv);
}
