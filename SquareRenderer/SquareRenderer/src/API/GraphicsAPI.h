#pragma once

#include "Common/Macros.h"

#include <string>

DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(GPUTimer);
DECLARE_PTRS(IUniformBlockResource);
DECLARE_PTRS(IGeometry);
DECLARE_PTRS(ShaderSource);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(ITexture);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IUniformBlockData);

class GPUTimer
{
public:
	GPUTimer();
	~GPUTimer();
	void begin();
	void end();
	unsigned long long fetchElapsedNs();
	double fetchElapsedMs();
private:
	unsigned int m_handle;
};

class GraphicsAPI
{
public:

	GraphicsAPI();

	virtual ~GraphicsAPI();

	bool allocate(IGeometrySPtr geometry);

	bool allocate(ITextureSPtr texture, const void* data = nullptr);

	bool allocate(RenderTargetSPtr rendertarget);

	bool allocate(IUniformBlockDataSPtr uniformBlockData);

	struct Result
	{
		bool success;
		std::string message;

		operator bool() const { return success; };
	};

	Result compile(ShaderProgramSPtr program);

	static GraphicsAPISPtr create();

	static bool checkError(const char* file, int line);

	struct ScopedDebugMarker
	{
		ScopedDebugMarker(const std::string& name);
		~ScopedDebugMarker();
	};

protected:

	Result compile(ShaderSourceSPtr shader);
};

#define GraphicsAPICheckError() GraphicsAPI::checkError(__FILE__, __LINE__) 

#ifdef _DEBUG
#define GraphicsAPIBeginScopedDebugGroup(name) const GraphicsAPI::ScopedDebugMarker scpDebGrp(name)
#else
#define GraphicsAPIBeginScopedDebugGroup(name)
#endif
