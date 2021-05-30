#include "Preprocessor/MaterialImporter.h"
#include "Preprocessor/ShaderParser.h"
#include "API/GraphicsAPI.h"
#include "Material/MaterialLibrary.h"
#include "Material/ShaderProgram.h"
#include "Common/Logger.h"
#include "Texture/Texture2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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


    // create default textures

    Texture2DSPtr defaultWhiteTex =
        std::make_shared<Texture2D>(1, 1, TextureFormat::RGBA);
    const glm::i8vec4 defaultWhiteVal(255, 255, 255, 255);
    if (m_api->allocate(defaultWhiteTex, glm::value_ptr(defaultWhiteVal)))
    {
        matLib->registerDefaultTexture("white", defaultWhiteTex);
    }

    Texture2DSPtr defaultBlackTex =
        std::make_shared<Texture2D>(1, 1, TextureFormat::RGBA);
    const glm::i8vec4 defaultBlackVal(0, 0, 0, 255);
    if (m_api->allocate(defaultBlackTex, glm::value_ptr(defaultBlackVal)))
    {
        matLib->registerDefaultTexture("black", defaultBlackTex);
    }

    Texture2DSPtr defaultNormalTex =
        std::make_shared<Texture2D>(1, 1, TextureFormat::RGB);
    const glm::i8vec3 defaultNormalVal(128, 128, 255);
    if (m_api->allocate(defaultNormalTex, glm::value_ptr(defaultNormalVal)))
    {
        matLib->registerDefaultTexture("normal", defaultNormalTex);
    }


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

            // set default textures
            for (const auto& [uniformName, metaInfo] : program->uniformMetaInfo())
            {
                if (metaInfo.type == UniformType::Texture2D)
                {
                    program->setUniformDefault(uniformName, defaultWhiteTex);
                }
            }
        }
    }

    return matLib;
}

ShaderProgramSPtr MaterialImporter::loadProgramFromFiles(
    const std::string& name, const std::vector<std::string>& shaderPaths)
{
    ShaderProgramSPtr program = std::make_shared<ShaderProgram>(name);
    for (const std::string& path : shaderPaths)
    {
        program->addShaderSource(m_shaderParser->loadFromFile(path));
    }

    auto result = m_api->compile(program);
    if (!result)
    {
        Logger::Error("Error while linking shader program: '%s'\n%s",
            program->name().c_str(), result.message.c_str());

        return nullptr;
    }
    return program;
}
