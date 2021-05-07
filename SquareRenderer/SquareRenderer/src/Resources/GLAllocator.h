#pragma once

#include <string>

class Geometry;
class ShaderSource;
class ShaderProgram;
class GLResource;

class GLAllocator
{
public:

	bool allocate(Geometry& geometry);

	struct Result
	{
		bool success;
		std::string message;

		operator bool() { return success; };
	};

	Result compile(ShaderSource& shader);

	Result link(ShaderProgram& program);
};

