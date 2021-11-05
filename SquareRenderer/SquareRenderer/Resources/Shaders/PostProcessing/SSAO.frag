#version 450 core

#pragma include ../Includes/Globals.glsl
#pragma include ../Includes/Common.glsl //! #include "../Includes/Common.glsl"
#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"
#pragma include ../Includes/Sampling.glsl //! #include "../Includes/Sampling.glsl"

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D normalsDepthTexture;
uniform float sampleCount = 10;
uniform float radius = 0.1;

void main()
{
    vec4 normalsDepth = texture(normalsDepthTexture, TexCoords).rgba;

    vec3 normal = normalsDepth.xyz;
    float depth = normalsDepth.w;

    float nDepth = _linearizeDepth( depth, _clip.x, _clip.y );

    vec3 posReconstructedWS = _worldPosFromDepth(depth, TexCoords, _invP, _invV);

//    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
//    vec3 bitangent = cross(normal, tangent);
//    mat3 TBN       = mat3(tangent, bitangent, normal); 
//
//    float occlusion = 0.0;
//    for(int i = 0; i < int(sampleCount); ++i)
//    {
//        // get sample position
//        vec3 samplePos = TBN * samples[i]; // from tangent to view-space
//        samplePos = posReconstructedWS + samplePos * radius; 
//    
//        [...]
//    }



    //FragColor = vec4(normalize(normalsDepth.xyz) * 0.5 + 0.5, 1.f);
    //FragColor = vec4(nDepth, nDepth, nDepth, 1.f);
    //FragColor = vec4(normal, 1.f);
    FragColor = vec4(abs(posReconstructedWS - normal) * 100, 1.f);
}