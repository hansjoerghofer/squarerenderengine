#pragma once

#include "Common/Macros.h"

#include <glm/glm.hpp>

DECLARE_PTRS(IViewport);

class IViewport
{
public:

	virtual int width() const = 0;

	virtual int height() const = 0;

	virtual const glm::vec4& viewport() const = 0;
};