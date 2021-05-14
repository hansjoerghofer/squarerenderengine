#include "Scene/LineSet.h"

LineSet::LineSet(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& colors)
	: m_vertices(pointsToVertices(points, colors))
{
}

LineSet::~LineSet()
{
}

bool LineSet::isStatic() const
{
	return true;
}

size_t LineSet::vertexCount() const
{
	return m_vertices.size();
}

size_t LineSet::vertexBufferSize() const
{
	return m_vertices.size() * sizeof(Vertex);
}

size_t LineSet::indexCount() const
{
	return 0;
}

size_t LineSet::indexBufferSize() const
{
	return 0;
}

const std::vector<Vertex>& LineSet::vertices() const
{
	return m_vertices;
}

const std::vector<uint32_t>& LineSet::indices() const
{
	return std::vector<uint32_t>();
}

bool LineSet::hasUVs() const
{
	return false;
}

bool LineSet::hasNormals() const
{
	return true;
}

bool LineSet::hasTangents() const
{
	return false;
}

std::vector<Vertex> LineSet::pointsToVertices(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& colors)
{
	if (points.size() != colors.size() * 2)
	{
		return std::vector<Vertex>();
	}

	std::vector<Vertex> vertices(points.size());

	for (int i = 0; i < points.size(); ++i)
	{
		vertices[i].position = points[i];
		vertices[i].normal = colors[i / 2];
	}

	return vertices;
}
