#include "API/SharedResource.h"

SharedResource::~SharedResource()
{
}

SharedResourceHandle SharedResource::handle() const
{
	return m_handle;
}

bool SharedResource::isValid() const
{
	return m_handle != INVALID_HANDLE;
}
