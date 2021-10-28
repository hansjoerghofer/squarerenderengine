#version 450 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float gamma = 2.2; // [1, 5]
uniform float exposure = 1.0; // [0, 10]

// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
// src: https://github.com/dmnsgn/glsl-tone-map/blob/master/aces.glsl
vec3 aces(vec3 x) 
{
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float aces(float x) 
{
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 reinhard(vec3 x)
{
    // reinhard tone mapping
    return x / (x + vec3(1));
}

// src: https://learnopengl.com/Advanced-Lighting/HDR
vec3 exposureTonemapping(vec3 x, float exposure)
{
    return vec3(1.0) - exp(-x * exposure);
}

void main()
{ 
    vec3 hdr = texture(screenTexture, TexCoords).rgb;

    vec3 toneMapped = aces(hdr);

    vec3 gammaCorrected = pow(toneMapped, vec3(1.0 / gamma));

    FragColor = vec4(gammaCorrected, 1.f);
}