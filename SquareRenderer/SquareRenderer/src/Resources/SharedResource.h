#pragma once

#include "Common/Macros.h"

DECLARE_PTRS(SharedResource);

typedef int SharedResourceHandle;

class SharedResource
{
public:

	virtual ~SharedResource() = 0;

	virtual int handle() const;

	virtual bool isValid() const;

protected:

	constexpr static const SharedResourceHandle INVALID_HANDLE = -1;

	SharedResourceHandle m_handle = INVALID_HANDLE;

};
