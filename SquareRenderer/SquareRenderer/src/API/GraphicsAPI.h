#pragma once

#include "Common/Macros.h"

#include <string>

DECLARE_PTRS(GraphicsAPI);
DECLARE_PTRS(IUniformBlockResource);
DECLARE_PTRS(Geometry);
DECLARE_PTRS(ShaderSource);
DECLARE_PTRS(ShaderProgram);
DECLARE_PTRS(ITexture);
DECLARE_PTRS(RenderTarget);
DECLARE_PTRS(IUniformBlockData);

class GraphicsAPI
{
public:

	GraphicsAPI();

	virtual ~GraphicsAPI();

	bool allocate(GeometrySPtr geometry);

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

protected:

	Result compile(ShaderSourceSPtr shader);
};

#define GraphicsAPICheckError() GraphicsAPI::checkError(__FILE__, __LINE__) 
