#version 330 core

#pragma include Includes/Camera.glsl //! #include "Includes/Camera.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vUVs;
layout (location = 2) in vec3 vNormal; // reuse the normal as a color input

out vec3 color;

void main()
{
    color = vNormal;
    gl_Position = _VP * vec4(vPosition, 1.0);
}
