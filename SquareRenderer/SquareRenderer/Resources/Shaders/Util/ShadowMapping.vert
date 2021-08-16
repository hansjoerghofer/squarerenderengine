#version 450 core

layout (location = 0) in vec3 vPosition;
//layout (location = 1) in vec2 vUVs;

layout (location = 0) uniform mat4 modelToWorld;
layout (location = 1) uniform mat4 worldToLight;

//out vec2 uvs;

void main()
{
    //uvs = vUVs;
    gl_Position = worldToLight * modelToWorld * vec4(vPosition, 1.0);
} 