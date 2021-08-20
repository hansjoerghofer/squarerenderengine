#version 450 core

#pragma include ../Includes/Common.glsl //! #include "../Includes/Common.glsl"
#pragma include ../Includes/PBR.glsl //! #include "../Includes/PBR.glsl"
#pragma include ../Includes/Lights.glsl //! #include "../Includes/Lights.glsl"
#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;

uniform mat4 modelToWorld = mat4(1);
uniform mat4 normalToWorld = mat4(1);

out VertexShaderData
{
    vec2 uv;
    vec3 normal;
    vec3 pFragmentWS;

    mat3 TBN;

    vec4[_MAX_SIZE_LIGHT] pFragmentLS;
} OUT;

void main() 
{
    vec4 fragPosWS = modelToWorld * vec4(vPosition, 1.0);

    OUT.uv = vUV;
    OUT.pFragmentWS = fragPosWS.xyz;
    
    OUT.TBN = _constructTBN(modelToWorld, vNormal, vTangent);
    OUT.normal = normalize(normalToWorld * vec4(vNormal, 0)).xyz;

    for(int i = 0; i < _numLights; ++i)
    {
        OUT.pFragmentLS[i] = _lightsMatrix[i] * fragPosWS;
    }

    gl_Position = _VP * fragPosWS;
}