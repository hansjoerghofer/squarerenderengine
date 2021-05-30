#pragma once

#include "Common/Macros.h"

DECLARE_PTRS(IWidget)

class IWidget
{
public:
	virtual void update(double deltaTime) = 0;

	virtual void draw() = 0;
};