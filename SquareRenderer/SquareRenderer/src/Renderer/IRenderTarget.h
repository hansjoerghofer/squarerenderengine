#pragma once

#include "API/SharedResource.h"
#include "Common/Macros.h"
#include "Common/Math3D.h"
#include <vector>

DECLARE_PTRS(IRenderTarget);

class IRenderTarget
{
public:

	virtual int width() const = 0;

	virtual int height() const = 0;

	virtual glm::vec4 dimensions() const = 0;

	virtual SharedResource::Handle handle() const = 0;
};