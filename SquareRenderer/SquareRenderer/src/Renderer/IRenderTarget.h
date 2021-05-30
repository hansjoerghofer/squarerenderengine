#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"

#include <glm/glm.hpp>

DECLARE_PTRS(IRenderTarget);

class IRenderTarget
{
public:

	virtual int width() const = 0;

	virtual int height() const = 0;

	virtual SharedResource::Handle handle() const = 0;
};