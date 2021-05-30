#pragma once

#include <glm/glm.hpp>

struct CameraUniformBlock
{
	alignas(16) glm::mat4 P;    // projection mat
	alignas(16) glm::mat4 invP; // inverse projection mat
	alignas(16) glm::mat4 V;    // view mat
	alignas(16) glm::mat4 invV; // inverse view mat
	alignas(16) glm::mat4 VP;   // view-projection mat
	alignas(16) glm::vec4 dim;  // resolution (w,h,1/w,1/h)
	alignas(16) glm::vec4 clip;  // clipping planes (n,f,1/n,1/f)
};

constexpr auto MAX_LIGHT_COUNT = 4;
struct LightsUniformBlock
{
	alignas(16) glm::vec4 ambientColor;

	alignas(16) int numLights;
	alignas(16) glm::vec4 lightsPosWS[MAX_LIGHT_COUNT];
	alignas(16) glm::vec4 lightsColor[MAX_LIGHT_COUNT];
};

bool operator==(const CameraUniformBlock& l, const CameraUniformBlock& r)
{
	return l.P == r.P && l.invP == r.invP &&
		l.V == r.V && l.invV == r.invV &&
		l.VP == r.VP &&
		l.dim == r.dim &&
		l.clip == r.clip;
}

bool operator!=(const CameraUniformBlock& l, const CameraUniformBlock& r)
{
	return !(l == r);
}

bool operator==(const LightsUniformBlock& l, const LightsUniformBlock& r)
{
	return l.ambientColor == r.ambientColor &&
		l.numLights == r.numLights &&
		l.lightsPosWS == r.lightsPosWS &&
		l.lightsColor == r.lightsColor;
}

bool operator!=(const LightsUniformBlock& l, const LightsUniformBlock& r)
{
	return !(l == r);
}