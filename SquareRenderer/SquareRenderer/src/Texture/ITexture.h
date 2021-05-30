#pragma once

#include "Common/Macros.h"
#include "Texture/TextureDefines.h"
#include "API/SharedResource.h"

DECLARE_PTRS(ITexture);

class ITexture
{
public:
    virtual ~ITexture() {};

    virtual TextureLayout layout() const = 0;

    virtual int width() const = 0;

    virtual int height() const = 0;

    virtual int layers() const = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;

    virtual SharedResource::Handle handle() const = 0;
};
