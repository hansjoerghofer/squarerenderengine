//? #version 450 core

#define _MAX_SIZE_LIGHT 1

layout(std140) uniform LightsUBO
{
	vec4 _ambientColor;

   int _numLights;
   vec4[_MAX_SIZE_LIGHT] _lightsPosWS;
   vec4[_MAX_SIZE_LIGHT] _lightsColor;

   // shadow mapping
   mat4[_MAX_SIZE_LIGHT] _lightsMatrix;
   int[_MAX_SIZE_LIGHT] _shadowMapIndex;
};