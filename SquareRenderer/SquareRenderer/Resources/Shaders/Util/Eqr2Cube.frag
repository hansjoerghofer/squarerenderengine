#version 450 core

layout (location = 0)  out vec4 Color;

in vec4 FragPos;

// ( 1/2PI, 1/PI )
const vec2 IATAN = vec2(0.1591, 0.3183);

uniform sampler2D equirectangularMap;
uniform float horizontalRotation = 0;

vec2 SampleSphericalMap(in vec3 direction)
{
    vec2 uv = vec2(atan(direction.z, direction.x), asin(direction.y));
    uv *= IATAN;

    // -z should point to center of equirectangular map
    uv += vec2(0.75, 0.5);

    return uv;
}

void main()
{		
    // make sure to normalize localPos
    vec3 direction = normalize(FragPos.xyz);
    vec2 uv = SampleSphericalMap(direction);

    //uv.s += horizontalRotation;

    vec3 color = texture(equirectangularMap, uv).rgb;

    Color = vec4(color, 1.0);
}