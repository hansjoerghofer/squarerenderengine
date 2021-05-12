#version 450 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;

uniform mat4 M;
uniform mat4 N;

out VSData
{
    vec2 uv;
    vec3 normalWS;
    vec3 fragPosWS;
} vs_out;

void main() 
{
    vec4 fragPosWS = M * vec4(vPosition, 1.0);

    vs_out.uv = vUV;
    vs_out.normalWS = normalize(N * vec4(vNormal, 0)).xyz;
    vs_out.fragPosWS = fragPosWS.xyz;

    gl_Position = _VP * fragPosWS;
}