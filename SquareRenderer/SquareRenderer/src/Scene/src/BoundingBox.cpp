#include "Scene/BoundingBox.h"

void BoundingBox::insert(const glm::vec3& point)
{
	for (int i = 0; i < 3; ++i)
	{
		if (point[i] > m_max[i]) m_max[i] = point[i];
		if (point[i] < m_min[i]) m_min[i] = point[i];
	}
}

void BoundingBox::merge(const BoundingBox& other)
{
	insert(other.m_min);
	insert(other.m_max);
}

const glm::vec3& BoundingBox::min() const
{
	return m_min;
}

const glm::vec3& BoundingBox::max() const
{
	return m_max;
}

bool BoundingBox::empty() const
{
    return m_min.x == MAXF &&
           m_min.y == MAXF &&
           m_min.z == MAXF &&
           m_max.x == MINF &&
           m_max.y == MINF &&
           m_max.z == MINF;
}

glm::vec3 BoundingBox::center() const
{
    return (m_min + m_max) * 0.5f;
}

glm::vec3 BoundingBox::size() const
{
    return m_max - m_min;
}

BoundingBox BoundingBox::transform(const glm::mat4& transform, const BoundingBox& aabb)
{
    // source: https://gamemath.com/book/geomprims.html#transforming_aabbs
    // Start with the last row of the matrix, which is the translation
    // portion, i.e. the location of the origin after transformation.
    BoundingBox out = BoundingBox();
    out.m_min = glm::vec3(transform[3]);
    out.m_max = out.m_min;

    if (transform[0][0] > 0.0f) {
        out.m_min.x += transform[0][0] * aabb.m_min.x; 
        out.m_max.x += transform[0][0] * aabb.m_max.x;
    }
    else {
        out.m_min.x += transform[0][0] * aabb.m_max.x; 
        out.m_max.x += transform[0][0] * aabb.m_min.x;
    }

    if (transform[0][1] > 0.0f) {
        out.m_min.y += transform[0][1] * aabb.m_min.x; 
        out.m_max.y += transform[0][1] * aabb.m_max.x;
    }
    else {
        out.m_min.y += transform[0][1] * aabb.m_max.x; 
        out.m_max.y += transform[0][1] * aabb.m_min.x;
    }

    if (transform[0][2] > 0.0f) {
        out.m_min.z += transform[0][2] * aabb.m_min.x; 
        out.m_max.z += transform[0][2] * aabb.m_max.x;
    }
    else {
        out.m_min.z += transform[0][2] * aabb.m_max.x; 
        out.m_max.z += transform[0][2] * aabb.m_min.x;
    }

    if (transform[1][0] > 0.0f) {
        out.m_min.x += transform[1][0] * aabb.m_min.y; 
        out.m_max.x += transform[1][0] * aabb.m_max.y;
    }
    else {
        out.m_min.x += transform[1][0] * aabb.m_max.y; 
        out.m_max.x += transform[1][0] * aabb.m_min.y;
    }

    if (transform[1][1] > 0.0f) {
        out.m_min.y += transform[1][1] * aabb.m_min.y; 
        out.m_max.y += transform[1][1] * aabb.m_max.y;
    }
    else {
        out.m_min.y += transform[1][1] * aabb.m_max.y; 
        out.m_max.y += transform[1][1] * aabb.m_min.y;
    }

    if (transform[1][2] > 0.0f) {
        out.m_min.z += transform[1][2] * aabb.m_min.y; 
        out.m_max.z += transform[1][2] * aabb.m_max.y;
    }
    else {
        out.m_min.z += transform[1][2] * aabb.m_max.y; 
        out.m_max.z += transform[1][2] * aabb.m_min.y;
    }

    if (transform[2][0] > 0.0f) {
        out.m_min.x += transform[2][0] * aabb.m_min.z; 
        out.m_max.x += transform[2][0] * aabb.m_max.z;
    }
    else {
        out.m_min.x += transform[2][0] * aabb.m_max.z; 
        out.m_max.x += transform[2][0] * aabb.m_min.z;
    }

    if (transform[2][1] > 0.0f) {
        out.m_min.y += transform[2][1] * aabb.m_min.z; 
        out.m_max.y += transform[2][1] * aabb.m_max.z;
    }
    else {
        out.m_min.y += transform[2][1] * aabb.m_max.z; 
        out.m_max.y += transform[2][1] * aabb.m_min.z;
    }

    if (transform[2][2] > 0.0f) {
        out.m_min.z += transform[2][2] * aabb.m_min.z; 
        out.m_max.z += transform[2][2] * aabb.m_max.z;
    }
    else {
        out.m_min.z += transform[2][2] * aabb.m_max.z; 
        out.m_max.z += transform[2][2] * aabb.m_min.z;
    }
    
    /*const glm::vec3 v000 = aabb.min();
    const glm::vec3 v111 = aabb.max();
    const glm::vec3 v001(v000.x, v000.y, v111.z);
    const glm::vec3 v010(v000.x, v111.y, v000.z);
    const glm::vec3 v100(v111.x, v000.y, v000.z);
    const glm::vec3 v011(v000.x, v111.y, v111.z);
    const glm::vec3 v101(v111.x, v000.y, v111.z);
    const glm::vec3 v110(v111.x, v111.y, v000.z);

    BoundingBox out = BoundingBox();
    out.insert(glm::vec3(transform * glm::vec4(v000, 1)));
    out.insert(glm::vec3(transform * glm::vec4(v001, 1)));
    out.insert(glm::vec3(transform * glm::vec4(v010, 1)));
    out.insert(glm::vec3(transform * glm::vec4(v011, 1)));
    out.insert(glm::vec3(transform * glm::vec4(v100, 1)));
    out.insert(glm::vec3(transform * glm::vec4(v101, 1)));
    out.insert(glm::vec3(transform * glm::vec4(v110, 1)));
    out.insert(glm::vec3(transform * glm::vec4(v111, 1)));*/

    return out;
}

BoundingBox operator*(const glm::mat4& transform, const BoundingBox& aabb)
{
    return BoundingBox::transform(transform, aabb);
}
