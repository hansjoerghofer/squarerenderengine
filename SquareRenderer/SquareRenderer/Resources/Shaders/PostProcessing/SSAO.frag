#version 450 core

#pragma include ../Includes/Common.glsl //! #include "../Includes/Common.glsl"
#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D normalsDepthTexture;

void main()
{
    vec4 normalsDepth = texture(normalsDepthTexture, TexCoords).rgba;

    vec3 normal = normalsDepth.xyz;
    float depth = normalsDepth.w;

    float nDepth = _linearizeDepth( depth, _clip.x, _clip.y );

    vec3 posReconstructedWS = _worldPosFromDepth(depth, TexCoords, _invP, _invV);

    //FragColor = vec4(normalize(normalsDepth.xyz) * 0.5 + 0.5, 1.f);
    //FragColor = vec4(nDepth, nDepth, nDepth, 1.f);
    //FragColor = vec4(normal, 1.f);
    FragColor = vec4(abs(posReconstructedWS - normal) * 100, 1.f);
}