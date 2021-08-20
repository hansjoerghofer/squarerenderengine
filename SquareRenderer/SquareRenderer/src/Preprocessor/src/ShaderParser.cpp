#include "Preprocessor/ShaderParser.h"
#include "Material/ShaderSource.h"
#include "Common/Logger.h"

#include <limits>
#include <fstream>
#include <exception>
#include <filesystem>
#include <map>
#include <regex>

static const std::map<std::string, ShaderType> s_extensionToShaderType =
{
    { ".vert", ShaderType::Vertex },
    { ".frag", ShaderType::Fragment },
    { ".geom", ShaderType::Geometry },
    { ".comp", ShaderType::Compute }
};

static const std::map<std::string, UniformType> s_glslTypeToUniformType =
{
    { "float",          UniformType::Float },
    { "int",            UniformType::Int },
    { "vec4",           UniformType::Vec4 },
    { "mat4",           UniformType::Mat4 },
    { "sampler2D",      UniformType::Texture2D },
    { "samplerCube",    UniformType::Cubemap }
};

static const std::regex s_uniformRegEx = std::regex("^.*uniform (\\w+) ([\\w_\\[\\]]+)" // type and name
                                                    "(?: = (.+))?;"                     // optional default                
                                                    "(?:\\s*\\/\\/\\s*(\\[.*\\]))?");   // optional range

static const std::regex s_rangeRegEx = std::regex("\\[\\s?([\\d\\.]+)\\s?,\\s?([\\d\\.]+)\\s?\\]"); // [x,y]
static const std::regex s_hintRegEx = std::regex("\\[\\s?(\\w+)\\s?\\]"); // [white]

ShaderSourceSPtr ShaderParser::loadFromFile(const std::string& filepath)
{
    std::filesystem::path path(filepath);

    const auto& foundShader = m_shaderCache.find(filepath);
    if (foundShader != m_shaderCache.end())
    {
        Logger::Info("Load shader from cache '%s'", filepath.c_str());
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
    m_tempUniformCache.clear();

    // load the file
    std::stringstream content;
    if (readFile(path, content))
    {
        ShaderSourceSPtr shader = std::make_shared<ShaderSource>(
            type, std::move(content.str()), m_tempUniformCache);

        m_shaderCache[filepath] = shader;

        Logger::Info("Load shader from file '%s'", filepath.c_str());

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
        else if (line.size() > 0 && line.rfind("uniform", 0) == 0)
        {
            ok &= handleUniform(line, out);
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

bool ShaderParser::handleUniform(const std::string& line, std::stringstream& out)
{
    out << line << '\n';

    // parse uniform meta info
    std::smatch match;
    if (std::regex_match(line, match, s_uniformRegEx) && match.size() > 2)
    {
        const std::string typeStr = match[1].str();
        const std::string name = match[2].str();
        const std::string defaultValueStr = match.size() > 3 ? match[3].str() : "";
        const std::string hintStr = match.size() > 4 ? match[4].str() : "";

        auto found = s_glslTypeToUniformType.find(typeStr);
        if (found == s_glslTypeToUniformType.end())
        {
            return true;
        }
        const UniformType type = found->second;

        std::string hintValuesStr[2];
        if (!hintStr.empty())
        {
            std::smatch matchHint;
            if (std::regex_match(hintStr, matchHint, s_rangeRegEx))
            {
                hintValuesStr[0] = matchHint[1].str();
                hintValuesStr[1] = matchHint[2].str();
            }
            else if (std::regex_match(hintStr, matchHint, s_hintRegEx))
            {
                hintValuesStr[0] = matchHint[1].str();
            }
        }
        const bool hasRange = !hintValuesStr[0].empty() && !hintValuesStr[1].empty();

        UniformMetaInfo metaInfo;
        metaInfo.name = name;
        metaInfo.type = found->second;
        metaInfo.defaultValue = UniformValue(metaInfo.type);

        constexpr float max = std::numeric_limits<float>::max();
        constexpr float min = -max;

        switch (metaInfo.type)
        {
        case UniformType::Int:
            try
            {
                metaInfo.defaultValue = UniformValue(std::stoi(defaultValueStr));
            }
            catch (...) {}

            if (hasRange)
            {
                metaInfo.minValue = UniformValue(std::stoi(hintValuesStr[0]));
                metaInfo.maxValue = UniformValue(std::stoi(hintValuesStr[1]));
            }
            else
            {
                metaInfo.minValue = UniformValue(std::numeric_limits<int>::min());
                metaInfo.maxValue = UniformValue(std::numeric_limits<int>::max());
            }            
            break;
        case UniformType::Float:
            try
            {
                metaInfo.defaultValue = UniformValue(std::stof(defaultValueStr));
            }
            catch (...) {}

            if (hasRange)
            {
                metaInfo.minValue = UniformValue(std::stof(hintValuesStr[0]));
                metaInfo.maxValue = UniformValue(std::stof(hintValuesStr[1]));
            }
            else if (name.find("Factor") != std::string::npos)
            {
                metaInfo.minValue = UniformValue(0.f);
                metaInfo.maxValue = UniformValue(1.f);
            }
            else
            {
                metaInfo.minValue = UniformValue(min);
                metaInfo.maxValue = UniformValue(max);
            }
            break;
        case UniformType::Vec4:
            if (name.find("Color") != std::string::npos)
            {
                metaInfo.minValue = UniformValue(glm::vec4(0, 0, 0, 0));
                metaInfo.maxValue = UniformValue(glm::vec4(1, 1, 1, 1));
                metaInfo.defaultValue = metaInfo.maxValue;
            }
            else
            {
                metaInfo.minValue = UniformValue(glm::vec4(min, min, min, min));
                metaInfo.maxValue = UniformValue(glm::vec4(max, max, max, max));
            }
            break;
        case UniformType::Texture2D:
            if (!hintValuesStr[0].empty())
            {
                metaInfo.hint = hintValuesStr[0];
            }
        }

        m_tempUniformCache.emplace_back(std::move(metaInfo));
    }
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
