#version 450 core

#pragma include ../Includes/Camera.glsl //! #include "../Includes/Camera.glsl"

layout (location = 0) in vec3 vPosition;

out vec3 uvs;

void main()
{
    uvs = vPosition;

    vec4 pos = _P * mat4(mat3(_V)) * vec4(vPosition, 1.0);
    gl_Position = pos.xyww;
} 