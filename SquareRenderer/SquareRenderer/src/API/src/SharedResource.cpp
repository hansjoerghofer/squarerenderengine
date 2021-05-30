#include "API/SharedResource.h"

SharedResource::~SharedResource()
{
}

SharedResource::Handle SharedResource::handle() const
{
	return m_handle;
}

bool SharedResource::isValid() const
{
	return m_handle != INVALID_HANDLE;
}
