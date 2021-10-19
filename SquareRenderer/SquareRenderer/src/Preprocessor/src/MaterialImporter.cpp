#include "Common/Logger.h"
#include "Common/Math3D.h"

#include "Preprocessor/MaterialImporter.h"
#include "Preprocessor/ShaderParser.h"
#include "API/GraphicsAPI.h"
#include "Material/MaterialLibrary.h"
#include "Material/ShaderProgram.h"
#include "Texture/Texture2D.h"

#include <yaml-cpp/yaml.h>

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
    /*std::ifstream file(path.c_str());
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file.");
    }*/

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

    YAML::Node config = YAML::LoadFile(filepath);

    //TODO use std::filesystem::path::preferred_separator ?
    //const std::string configRoot = std::filesystem::absolute(path).parent_path().string() + "/";
    
    const std::string shaderRoot = config["shaderRoot"].as<std::string>();

    for (const auto& programConf : config["programs"])
    {
        const std::string programName = programConf["name"].as<std::string>();

        std::vector<std::string> defines;
        for (const auto& defineNode : programConf["defines"])
        {
            defines.push_back(defineNode.as<std::string>());
        }

        std::vector<std::string> shaderPaths;
        for (const auto& pathNode : programConf["files"])
        {
            const std::string shaderPath = pathNode.as<std::string>();
            if (!shaderPath.empty())
            {
                shaderPaths.push_back(shaderRoot + shaderPath);
            }  
        }

        if (shaderPaths.empty()) continue;

        ShaderProgramSPtr program = loadProgramFromFiles(programName, shaderPaths, defines);
        
        if (!program) continue;

        matLib->registerProgram(program);

        // set defaults from shader meta information
        for (const auto& [uniformName, metaInfo] : program->uniformMetaInfo())
        {
            switch (metaInfo.type)
            {
            case UniformType::Float:
            case UniformType::Int:
            case UniformType::Vec4:
            case UniformType::Mat4:
                program->setUniformDefault(uniformName, UniformValue(metaInfo.defaultValue));
                break;
            case UniformType::Texture2D:
            case UniformType::Cubemap:
            {
                ITextureSPtr tex = matLib->findDefaultTexture(metaInfo.hint);
                if (tex)
                {
                    program->setUniformDefault(uniformName, tex);
                }
            }
            }
        }
    }


    /*std::string line;
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
        if (!program)
        {
            continue;
        }

        matLib->registerProgram(program);

        // set defaults from shader meta information
        for (const auto& [uniformName, metaInfo] : program->uniformMetaInfo())
        {
            switch (metaInfo.type)
            {
            case UniformType::Float:
            case UniformType::Int:
            case UniformType::Vec4:
            case UniformType::Mat4:
                program->setUniformDefault(uniformName, UniformValue(metaInfo.defaultValue));
                break;
            case UniformType::Texture2D:
            case UniformType::Cubemap:
            {
                ITextureSPtr tex = matLib->findDefaultTexture(metaInfo.hint);
                if (tex)
                {
                    program->setUniformDefault(uniformName, tex);
                }
            }
            }
        }
    }*/

    return matLib;
}

ShaderProgramSPtr MaterialImporter::loadProgramFromFiles(
    const std::string& name, 
    const std::vector<std::string>& shaderPaths,
    const std::vector<std::string>& defines )
{
    ShaderProgramSPtr program = std::make_shared<ShaderProgram>(name);
    for (const std::string& path : shaderPaths)
    {
        program->addShaderSource(m_shaderParser->loadFromFile(path, defines));
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
