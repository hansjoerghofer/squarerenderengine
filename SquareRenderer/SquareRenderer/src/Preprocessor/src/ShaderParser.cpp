#include "Preprocessor/ShaderParser.h"
#include "Material/ShaderSource.h"

#include <fstream>
#include <exception>
#include <filesystem>
#include <map>

static const std::map<std::string, ShaderType> s_extensionToShaderType =
{
    { ".vert", ShaderType::Vertex },
    { ".frag", ShaderType::Fragment },
    { ".geom", ShaderType::Geometry },
    { ".comp", ShaderType::Compute }
};

ShaderSourceSPtr ShaderParser::loadFromFile(const std::string& filepath)
{
    std::filesystem::path path(filepath);

    const auto& foundShader = m_shaderCache.find(filepath);
    if (foundShader != m_shaderCache.end())
    {
        return foundShader->second;
    }

    if (!std::filesystem::exists(path) || !path.has_filename())
    {
        throw std::runtime_error("Invalid path to Shader file.");
    }

    const auto& foundExt = s_extensionToShaderType.find(path.extension().string());
    if (foundExt == s_extensionToShaderType.end())
    {
        throw std::runtime_error("Unknown Shader extension.");
    }
    const ShaderType type = foundExt->second;

    // reset the path stack
    m_pathStack = std::stack<std::filesystem::path>();

    // load the file
    std::stringstream content;
    if (readFile(path, content))
    {
        ShaderSourceSPtr shader = std::make_shared<ShaderSource>(
            type, std::move(content.str()));

        m_shaderCache[filepath] = shader;

        return shader;
    }
    else
    {
        throw std::runtime_error("Could not load Shader file.");
    }
}

bool ShaderParser::readFile(const std::filesystem::path& filepath, std::stringstream& out)
{
    std::filesystem::path path = resolveCurrentPath(filepath);

    std::string pathStr = path.string();

    // check if file already exists in cache
    const auto& found = m_fileCache.find(pathStr);
    if (found != m_fileCache.end())
    {
        out << found->second;
        return true;
    }

    // open file
    std::ifstream file(pathStr);
    if (!file.is_open())
    {
        out << "// Error: could not open file '" << pathStr << "'.\n";
        return false;
    }

    // read file from disk
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    file.close();

    m_pathStack.push(path.parent_path());

    std::stringstream parsed;
    const bool valid = parseString(std::move(fileStream), parsed);

    m_pathStack.pop();

    if (!valid)
    {
        out << "// Error: Failded to parse file '" << pathStr << "'.\n";
        return false;
    }

    std::string contents = parsed.str();

    // append file contents to stream
    out << contents << '\n';

    // cache file contents
    m_fileCache.emplace(std::make_pair(
        std::move(pathStr), std::move(contents)));

    return true;
}

bool ShaderParser::parseString(std::stringstream&& source, std::stringstream& out)
{
    bool ok = true;

    std::string line;
    while (ok && std::getline(source, line))
    {
        // TODO trim line before check?
        if (line.size() > 0 && line[0] == '#')
        {
            ok &= handleDefine(line, out);
        }
        else
        {
            out << line << '\n';
        }
    }

    return ok;
}

bool ShaderParser::handleDefine(const std::string& line, std::stringstream& out)
{
    std::stringstream lineStream(line);
    std::string token, keyword;
    lineStream >> token >> keyword;

    if (token == "#pragma" && keyword == "include")
    {
        std::string value;
        lineStream >> value;

        if (m_addMetaComments)
        {
            out << "// >> include " << value << '\n';
        }

        // append include file into corrent string
        std::filesystem::path includePath(value);
        const bool ok = readFile(includePath, out);

        if (m_addMetaComments)
        {
            out << "// << include " << value << '\n';
        }
        return true;
    }

    out << line << '\n';
    return true;
}

std::filesystem::path ShaderParser::resolveCurrentPath(const std::filesystem::path& path) const
{
    if (path.is_relative())
    {
        if (m_pathStack.empty())
        {
            return std::filesystem::absolute(path);
        }
        else
        {
            return std::filesystem::path(m_pathStack.top()) / path;
        }
    }
    else
    {
        return path;
    }
}
