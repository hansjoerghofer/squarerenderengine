#pragma once

#include "Common/Macros.h"
#include "Geometry.h"

#include <glm/glm.hpp>

#include <vector>

DECLARE_PTRS(LineSet);

class LineSet : public Geometry
{
public:

    LineSet(const std::vector<glm::vec3>& points,
        const std::vector<glm::vec3>& colors);

    virtual ~LineSet() override;

    virtual bool isStatic() const override;

    virtual size_t vertexCount() const override;

    virtual size_t vertexBufferSize() const override;

    virtual size_t indexCount() const override;

    virtual size_t indexBufferSize() const override;

    virtual const std::vector<Vertex>& vertices() const override;

    virtual const std::vector<uint32_t>& indices() const override;

    virtual bool hasUVs() const override;

    virtual bool hasNormals() const override;

    virtual bool hasTangents() const override;

protected:

    static std::vector<Vertex> pointsToVertices(
        const std::vector<glm::vec3>& points,
        const std::vector<glm::vec3>& colors);

    std::vector<Vertex> m_vertices;
};