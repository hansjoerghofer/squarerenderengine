#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

#include <limits>

class BoundingBox
{
public:

	void insert(const glm::vec3& point);

	void merge(const BoundingBox& other);

	const glm::vec3& min() const;

	const glm::vec3& max() const;

	bool empty() const;

	glm::vec3 center() const;

	glm::vec3 size() const;

	static BoundingBox transform(const glm::mat4& transform, const BoundingBox& aabb);

private:

	constexpr static float MAXF = std::numeric_limits<float>::max();
	constexpr static float MINF = -MAXF;

	glm::vec3 m_min = glm::vec3(MAXF, MAXF, MAXF);
	glm::vec3 m_max = glm::vec3(MINF, MINF, MINF);
};

BoundingBox operator*(const glm::mat4& transform, const BoundingBox& aabb);
