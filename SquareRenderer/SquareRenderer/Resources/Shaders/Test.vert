#version 330 core

#pragma include Includes/Camera.glsl //! #include "Includes/Camera.glsl"

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
  
out vec3 vertexColor;

uniform mat4 M;

void main()
{
    gl_Position = _VP * M * vec4(pos, 1.0);
    vertexColor = vec3(uv.x, uv.y, 0.0);
}
