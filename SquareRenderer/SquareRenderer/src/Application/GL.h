#pragma once

#include "Common/Logger.h"

#include <glad/glad.h>

#include <string>

struct GLApplicationScope
{
	GLApplicationScope();

	~GLApplicationScope();
};

struct GLHelpers
{
    static GLenum checkError(const char* file, int line);
};

#define glCheckError() GLHelpers::checkError(__FILE__, __LINE__) 
