#version 450 core

#pragma include ../Includes/Common.glsl //! #include "../Includes/Common.glsl"

in VertexShaderData
{
    vec2 uv;
    mat3 TBN;
} IN;

layout (location = 0) out vec4 OutputNormalDepth;

// uniforms
uniform sampler2D normalMap; // [normal]

vec3 sampleNormal()
{
    vec3 normal = texture(normalMap, IN.uv).xyz;
    normal = normalize(normal * 2.0 - 1.0);   
    return normalize(IN.TBN * normal);
    //return normalize(IN.normal);
}

void main() 
{
    vec3 N = sampleNormal();
    
    OutputNormalDepth = vec4(N.xyz, gl_FragCoord.z);
}