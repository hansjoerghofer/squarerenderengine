#pragma once

#include "Common/Macros.h"

#include <string>

class Geometry;
class ShaderSource;
class ShaderProgram;

DECLARE_PTRS(UniformBlockResource);
DECLARE_PTRS(GraphicsAPI);

class GraphicsAPI
{
public:

	GraphicsAPI();

	virtual ~GraphicsAPI();

	bool allocate(Geometry& geometry);

	struct Result
	{
		bool success;
		std::string message;

		operator bool() const { return success; };
	};

	Result compile(ShaderProgram& program);

	UniformBlockResourceUPtr allocateUniformBlock(int location, size_t size);

	static GraphicsAPISPtr create();

	static bool checkError(const char* file, int line);

protected:

	Result compile(ShaderSource& shader);
};

#define GraphicsAPICheckError() GraphicsAPI::checkError(__FILE__, __LINE__) 
