#pragma once

#include "Common/Macros.h"

#include <vector>

DECLARE_PTRS(ILightsource);
struct LineSegment;
class BoundingBox;

class GizmoHelper
{
public:
	static float COORDIANTES_SIZE;
	static float POINT_LIGHT_SIZE;
	static float DIRECTIONAL_LIGHT_LENTGH;

	static std::vector<LineSegment> createCoordinateAxis();

	static std::vector<LineSegment> createLightsource(ILightsourceSPtr light);

	static std::vector<LineSegment> createBoundingBox(const BoundingBox& box);
};

