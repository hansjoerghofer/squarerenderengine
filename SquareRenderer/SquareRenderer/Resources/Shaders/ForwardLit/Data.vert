#version 450 core

#pragma include ../Includes/Common.glsl //! #include "../Includes/Common.glsl"
#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec3 vTangent;

out VertexShaderData
{
    vec2 uv;
    mat3 TBN;
} OUT;

uniform mat4 modelToWorld = mat4(1);
uniform mat4 normalToWorld = mat4(1);

void main() 
{
    vec4 fragPosWS = modelToWorld * vec4(vPosition, 1.0);

    OUT.uv = vUV;
    OUT.TBN = _constructTBN(modelToWorld, vNormal, vTangent);

    gl_Position = _VP * fragPosWS;
}