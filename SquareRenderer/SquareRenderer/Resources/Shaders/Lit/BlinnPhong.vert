#version 450 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

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
} vs_out;

void main() 
{
    vec4 fragPosWS = modelToWorld * vec4(vPosition, 1.0);

    vs_out.uv = vUV;
    vs_out.normalWS = normalize(normalToWorld * vec4(vNormal, 0)).xyz;
    vs_out.fragPosWS = fragPosWS.xyz;

    gl_Position = _VP * fragPosWS;
}