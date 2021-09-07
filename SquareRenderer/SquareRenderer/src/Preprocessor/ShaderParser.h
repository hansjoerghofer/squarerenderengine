#pragma once

#include "Common/Macros.h"

#include <string>
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <stack>

DECLARE_PTRS(ShaderSource);
struct UniformMetaInfo;

class ShaderParser
{
public:

	ShaderSourceSPtr loadFromFile(const std::string& filepath, const std::vector<std::string>& defines = {});

protected:

	bool readFile(const std::filesystem::path& filepath, std::stringstream& out);

	bool parseString(std::stringstream&& source, std::stringstream& out);

	bool handleDefine(const std::string& line, std::stringstream& out);

	bool handleUniform(const std::string& line, std::stringstream& out);

	std::filesystem::path resolveCurrentPath(const std::filesystem::path& path) const;

	bool m_addMetaComments = true;

	std::stack<std::filesystem::path> m_pathStack;

	std::unordered_map<std::string, std::string> m_fileCache;

	std::unordered_map<std::string, ShaderSourceSPtr> m_shaderCache;

	std::vector<UniformMetaInfo> m_tempUniformCache;

	std::vector<std::string> m_shaderDefines;
};

