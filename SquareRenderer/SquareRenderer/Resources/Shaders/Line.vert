#version 330 core

#pragma include Includes/Camera.glsl //! #include "Includes/Camera.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

out vec3 color;

void main()
{
    color = vColor;
    gl_Position = _VP * vec4(vPosition, 1.0);
}
