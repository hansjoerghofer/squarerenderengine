#version 450 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"
#pragma include ../Includes/Lights.glsl //! #include "../Includes/Lights.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;

uniform mat4 modelToWorld = mat4(1);
uniform mat4 normalToWorld = mat4(1);

out VSData
{
    vec2 uv;
    vec3 normalWS;
    vec3 fragPosWS;

    vec4[_MAX_SIZE_LIGHT] fragPosLS;
} vs_out;

void main() 
{
    vec4 fragPosWS = modelToWorld * vec4(vPosition, 1.0);

    vs_out.uv = vUV;
    vs_out.normalWS = normalize(normalToWorld * vec4(vNormal, 0)).xyz;
    vs_out.fragPosWS = fragPosWS.xyz;

    for(int i = 0; i < _numLights; ++i)
    {
        vs_out.fragPosLS[i] = _lightsMatrix[i] * fragPosWS;
    }

    gl_Position = _VP * fragPosWS;
}