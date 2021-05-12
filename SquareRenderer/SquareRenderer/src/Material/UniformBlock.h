#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"

DECLARE_PTRS(UniformBlockResource);

class UniformBlockResource : public SharedResource
{
public:
	
	virtual ~UniformBlockResource() = 0 {};

	virtual int bindingPoint() const = 0;

	virtual void update(const char* data) = 0;
};