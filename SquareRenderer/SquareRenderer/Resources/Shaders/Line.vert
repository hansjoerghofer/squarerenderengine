#version 330 core

#pragma include Includes/Camera.glsl //! #include "Includes/Camera.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

out vec3 color;

uniform mat4 modelToWorld = mat4(1);

void main()
{
    color = vColor.rgb;
    gl_Position = _VP * modelToWorld * vec4(vPosition.xyz, 1.0);
}
