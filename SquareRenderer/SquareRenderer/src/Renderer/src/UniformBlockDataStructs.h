#pragma once

#include "Common/Math3D.h"

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

#pragma warning( push )
#pragma warning( disable : 4324 )

constexpr auto MAX_LIGHT_COUNT = 2;
struct LightsUniformBlock
{
	alignas(16) glm::vec4 ambientColor;

	alignas(4) int numLights;							// warning C4324: 'LightsUniformBlock': structure was padded due to alignment specifier
	alignas(16) glm::vec4 lightsPosWS[MAX_LIGHT_COUNT];
	alignas(16) glm::vec4 lightsColor[MAX_LIGHT_COUNT];

	// shadow mapping
	alignas(16) glm::mat4 lightsMatrix[MAX_LIGHT_COUNT];
	alignas(16) glm::vec4 shadowMapIndex[MAX_LIGHT_COUNT]; // TODO check, using scalar does not work as expected!
};

#pragma warning( pop )

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

template<typename T>
bool cmpArray(const T* l, const T* r, size_t size)
{
	bool same = true;
	for (size_t i = 0; i < size; ++i)
	{
		same = same && l[i] == r[i];
	}
	return same;
}

bool operator==(const LightsUniformBlock& l, const LightsUniformBlock& r)
{
	return l.numLights == r.numLights && 
		l.ambientColor == r.ambientColor &&
		cmpArray(l.lightsPosWS, r.lightsPosWS, MAX_LIGHT_COUNT) &&
		cmpArray(l.lightsColor, r.lightsColor, MAX_LIGHT_COUNT) &&
		cmpArray(l.lightsMatrix, r.lightsMatrix, MAX_LIGHT_COUNT) &&
		cmpArray(l.shadowMapIndex, r.shadowMapIndex, MAX_LIGHT_COUNT);
}

bool operator!=(const LightsUniformBlock& l, const LightsUniformBlock& r)
{
	return !(l == r);
}