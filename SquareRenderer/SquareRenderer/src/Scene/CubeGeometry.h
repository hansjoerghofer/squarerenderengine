#pragma once

#include "Common/Macros.h"
#include "IGeometry.h"

DECLARE_PTRS(CubeGeometry);

class CubeGeometry : public IGeometry
{
public:

    CubeGeometry();

    virtual ~CubeGeometry() {};

    virtual size_t getVertexCount() const override;

    virtual size_t getVertexBufferSize() const override;

    virtual size_t getIndexCount() const override;

    virtual size_t getIndexBufferSize() const override;

    virtual const std::vector<Vertex>& getVertices() const override;

    virtual const std::vector<uint32_t>& getIndices() const override;

    static CubeGeometryUPtr create();

protected:

    static const std::vector<Vertex> vertices;

    static const std::vector<uint32_t> indices;
};
