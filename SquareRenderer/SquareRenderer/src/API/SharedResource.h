#pragma once

#include "Common/Macros.h"
#include "Common/Math3D.h"

#include <string>

DECLARE_PTRS(ITextureResource);
DECLARE_PTRS(IGeometryResource);
DECLARE_PTRS(IShaderResource);
DECLARE_PTRS(IShaderProgramResource);
DECLARE_PTRS(IUniformBlockResource);
DECLARE_PTRS(IRenderTargetResource);
DECLARE_PTRS(IDepthBufferResource);

class SharedResource
{
public:

	virtual ~SharedResource() = 0;

	typedef int Handle;
	constexpr static const Handle INVALID_HANDLE = -1;

	virtual Handle handle() const;

	virtual bool isValid() const;

protected:

	Handle m_handle = INVALID_HANDLE;

};

class IShaderResource : public SharedResource
{
};

class IUniformBlockResource : public SharedResource
{
public:

	virtual int bindingPoint() const = 0;

	virtual bool update(const char* data) = 0;
};

class IBindableResource : public SharedResource
{
public:

	virtual void bind() = 0;

	virtual void unbind() = 0;
};

class ITextureResource : public IBindableResource
{
public:

	virtual void update(int width, int height, 
		const void* data) = 0;

	virtual void generateMipmaps() = 0;

};

class IGeometryResource : public IBindableResource
{
};

class IShaderProgramResource : public IBindableResource
{
public:

	virtual int uniformLocation(const std::string& name) const = 0;

	virtual void setUniform(int location, int value) = 0;
	virtual void setUniform(int location, float value) = 0;
	virtual void setUniform(int location, const glm::vec4& value) = 0;
	virtual void setUniform(int location, const glm::mat4& value) = 0;

	virtual bool bindUniformBlock(const std::string& name, int binding) = 0;
};

class IRenderTargetResource : public SharedResource
{
};

class IDepthBufferResource : public SharedResource
{
};
