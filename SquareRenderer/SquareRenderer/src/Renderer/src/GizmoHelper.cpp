#include "Common/Math3D.h"

#include "Renderer/GizmoHelper.h"
#include "Scene/IGeometry.h"
#include "Scene/BoundingBox.h"
#include "Scene/ILightsource.h"
#include "Scene/DirectionalLight.h"
#include "Scene/PointLight.h"

constexpr glm::vec3 O(0, 0, 0);
constexpr glm::vec3 X(1, 0, 0);
constexpr glm::vec3 Y(0, 1, 0);
constexpr glm::vec3 Z(0, 0, 1);

float GizmoHelper::COORDIANTES_SIZE = 10.f;
float GizmoHelper::POINT_LIGHT_SIZE = 0.05f;
float GizmoHelper::DIRECTIONAL_LIGHT_LENTGH = 0.8f;

std::vector<LineSegment> GizmoHelper::createCoordinateAxis()
{
	return {
		{ O, X * COORDIANTES_SIZE, X },
		{ O, Y * COORDIANTES_SIZE, Y },
		{ O, Z * COORDIANTES_SIZE, Z }
	};
}

std::vector<LineSegment> GizmoHelper::createLightsource(ILightsourceSPtr light)
{
	static const glm::vec3 LightColor = glm::vec3(1, 1, 0);

	switch (light->type())
	{
	case LightsourceType::Directional:
	{
		const glm::vec3& direction = std::static_pointer_cast<DirectionalLight>(light)->direction();

		return { { -direction, O - direction * DIRECTIONAL_LIGHT_LENTGH, LightColor } };
	}
	case LightsourceType::Point:
	{
		const glm::vec3& position = std::static_pointer_cast<PointLight>(light)->position();

		return {
			{ position - X * POINT_LIGHT_SIZE, position + X * POINT_LIGHT_SIZE, LightColor },
			{ position - Y * POINT_LIGHT_SIZE, position + Y * POINT_LIGHT_SIZE, LightColor },
			{ position - Z * POINT_LIGHT_SIZE, position + Z * POINT_LIGHT_SIZE, LightColor }
		};
	}
	}

	return {};
}

std::vector<LineSegment> GizmoHelper::createBoundingBox(const BoundingBox& box)
{
	const glm::vec3 boxColor(1, 0, 1);

	const glm::vec3 v000 = box.min();
	const glm::vec3 v111 = box.max();
	const glm::vec3 v001(v000.x, v000.y, v111.z);
	const glm::vec3 v010(v000.x, v111.y, v000.z);
	const glm::vec3 v100(v111.x, v000.y, v000.z);
	const glm::vec3 v011(v000.x, v111.y, v111.z);
	const glm::vec3 v101(v111.x, v000.y, v111.z);
	const glm::vec3 v110(v111.x, v111.y, v000.z);

	return {
		{ v000, v001, boxColor },
		{ v000, v010, boxColor },
		{ v000, v100, boxColor },

		{ v111, v011, boxColor },
		{ v111, v110, boxColor },
		{ v111, v101, boxColor },

		{ v001, v011, boxColor },
		{ v010, v110, boxColor },
		{ v100, v101, boxColor },

		{ v001, v101, boxColor },
		{ v100, v110, boxColor },
		{ v010, v011, boxColor },
	};
}

std::vector<Vertex> GizmoHelper::linesToPrimitives(const std::vector<LineSegment>& lines)
{
	std::vector<Vertex> vertices;
	vertices.reserve(lines.size() * 2);
	for (const LineSegment& line : lines)
	{
		vertices.push_back({ line.start, {0,0}, line.color, {0,0,0} });
		vertices.push_back({ line.end, {0,0}, line.color, {0,0,0} });
	}
	return vertices;
}
