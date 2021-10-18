#pragma once

#include "Common/Macros.h"

#include <string>

DECLARE_PTRS(IWidget)

class IWidget
{
public:
	virtual const std::string& name() const = 0;

	virtual bool visible() const = 0;

	virtual void setVisible(bool flag) = 0;

	virtual void update(double deltaTime) = 0;

	virtual void draw() = 0;
};