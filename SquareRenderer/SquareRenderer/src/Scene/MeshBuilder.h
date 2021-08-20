#pragma once

#include "Common/Macros.h"

DECLARE_PTRS(IGeometry);

class MeshBuilder
{
public:

	static IGeometryUPtr cube();

	static IGeometryUPtr screenTriangle();

	static IGeometryUPtr skybox();
};

