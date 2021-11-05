#pragma once

#pragma warning( push )
#pragma warning( disable : 4201 )
#pragma warning( disable : 4127 )

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp >

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma warning( pop )

namespace glm
{
	constexpr vec3 vec3_right(1, 0, 0);
	constexpr vec3 vec3_up(0, 1, 0);
	constexpr vec3 vec3_forward(0, 0, 1);

	constexpr vec4 vec4_white(1, 1, 1, 1);
	constexpr vec4 vec4_black(0, 0, 0, 1);
}