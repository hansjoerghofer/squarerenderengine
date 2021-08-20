#pragma once

#include "Common/Macros.h"

DECLARE_PTRS(IGeometryVisitor);

class Mesh;
class PrimitiveSet;
class Geometry;

class IGeometryVisitor
{
public:

	virtual void visit(Mesh& mesh) = 0;

	virtual void visit(PrimitiveSet& primitiveSet) = 0;

};