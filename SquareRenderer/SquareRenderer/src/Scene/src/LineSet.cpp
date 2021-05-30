#include "Scene/LineSet.h"

LineSet::LineSet(const std::vector<LineSegment>& lines)
	: m_vertices(linesToVertices(lines))
	, m_indices()
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
	return m_indices;
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

std::vector<Vertex> LineSet::linesToVertices(const std::vector<LineSegment>& lines)
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
