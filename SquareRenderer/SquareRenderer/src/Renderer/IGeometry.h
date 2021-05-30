#pragma once

DECLARE_PTRS(IGeometry);

class IGeometry
{
public:

    virtual size_t vertexCount() const = 0;

    virtual size_t indexCount() const = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;
};