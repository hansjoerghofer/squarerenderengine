#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

#include <vector>

DECLARE_PTRS(ILightsource);
class BoundingBox;
struct Vertex;

struct LineSegment
{
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 color;
};

class GizmoHelper
{
public:
	static float COORDIANTES_SIZE;
	static float POINT_LIGHT_SIZE;
	static float DIRECTIONAL_LIGHT_LENTGH;

	static std::vector<LineSegment> createCoordinateAxis();

	static std::vector<LineSegment> createLightsource(ILightsourceSPtr light);

	static std::vector<LineSegment> createBoundingBox(const BoundingBox& box);

	static std::vector<Vertex> linesToPrimitives(const std::vector<LineSegment>& lines);
};

