#pragma once

#include "Common/Macros.h"

DECLARE_PTRS(IGeometry);
DECLARE_PTRS(Material);
DECLARE_PTRS(IDrawable);

class IDrawable
{
public:
	virtual IGeometrySPtr geometry() const = 0;

	virtual MaterialSPtr material() const = 0;

	virtual void preRender(MaterialSPtr boundMaterial) = 0;

	virtual void postRender() = 0;
};