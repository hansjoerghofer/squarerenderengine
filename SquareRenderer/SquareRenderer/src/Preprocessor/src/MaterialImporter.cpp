#include "Preprocessor/MaterialImporter.h"
#include "Preprocessor/ShaderParser.h"
#include "API/GraphicsAPI.h"
#include "Material/MaterialLibrary.h"
#include "Material/ShaderProgram.h"
#include "Common/Logger.h"

#include <fstream>
#include <exception>
#include <filesystem>

MaterialImporter::MaterialImporter(GraphicsAPISPtr api)
    : m_api(api)
    , m_shaderParser(new ShaderParser())
{
}

MaterialImporter::~MaterialImporter()
{
}

MaterialLibraryUPtr MaterialImporter::importFromFile(const std::string& filepath)
{
    std::filesystem::path path(filepath);

    if (!std::filesystem::exists(path) || !path.has_filename())
    {
        throw std::runtime_error("Invalid path to Material library definition.");
    }

    // open file
    std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file.");
    }

    MaterialLibraryUPtr matLib = MaterialLibrary::create();

    std::string root = path.parent_path().string() + "/";

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        
        std::string programName;
        ss >> programName;
        if (programName.empty()) continue;

        std::string shaderPathStr;
        std::vector<std::string> shaderPaths;
        while (ss >> shaderPathStr)
        {
            if (shaderPathStr.empty()) continue;

            shaderPaths.push_back(root + shaderPathStr);
        }

        ShaderProgramSPtr program = loadProgramFromFiles(programName, shaderPaths);
        if (program)
        {
            matLib->registerProgram(program);
        }
    }

    return matLib;
}

ShaderProgramUPtr MaterialImporter::loadProgramFromFiles(
    const std::string& name, const std::vector<std::string>& shaderPaths)
{
    ShaderProgramUPtr program = std::make_unique<ShaderProgram>(name);
    for (const std::string& path : shaderPaths)
    {
        program->addShaderSource(m_shaderParser->loadFromFile(path));
    }

    auto result = m_api->compile(*program);
    if (!result)
    {
        Logger::Error("Error while linking shader program: '%s'\n%s",
            program->name().c_str(), result.message.c_str());

        return nullptr;
    }
    return program;
}
