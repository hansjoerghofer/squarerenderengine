#include "Preprocessor/TextureImporter.h"
#include "Texture/Texture2D.h"
#include "API/GraphicsAPI.h"
#include "Common/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>

TextureFormat getFormat(int channels, bool sRGB, bool floatPrecision)
{
    switch (channels)
    {
    case 1: return floatPrecision ? TextureFormat::RFloat : TextureFormat::R;
    case 2: return floatPrecision ? TextureFormat::RGFloat : TextureFormat::RG;
    case 3: return floatPrecision ? TextureFormat::RGBFloat : sRGB ? TextureFormat::SRGB : TextureFormat::RGB;
    case 4:
    default:
        return floatPrecision ? TextureFormat::RGBAFloat : sRGB ? TextureFormat::SRGBA : TextureFormat::RGBA;
    }
}

std::pair<int, bool> getChannelCountAndColorspace(TextureImporter::ImportFormat format)
{
    int channels = 0;
    switch (format)
    {
    case TextureImporter::ImportFormat::R:
        channels = 1;
        break;
    case TextureImporter::ImportFormat::RG:
        channels = 2;
        break;
    case TextureImporter::ImportFormat::RGB:
    case TextureImporter::ImportFormat::sRGB:
        channels = 3;
        break;
    case TextureImporter::ImportFormat::RGBA:
    case TextureImporter::ImportFormat::sRGBA:
        channels = 4;
        break;
    }

    const bool sRGB = format == TextureImporter::ImportFormat::Auto ||
                      format == TextureImporter::ImportFormat::sRGB ||
                      format == TextureImporter::ImportFormat::sRGBA;

    return { channels, sRGB };
}

TextureImporter::TextureImporter(GraphicsAPISPtr api)
    : m_api(api)
{
}

TextureImporter::~TextureImporter()
{
    waitForCompletion();
}

Texture2DSPtr TextureImporter::importFromFile(
    const std::string& filePath, 
    ImportFormat format,
    TextureSampler sampler)
{
    ImageWrapper image = loadRawImageFile(filePath, format);
    if (!image.data)
    {
        return nullptr;
    }

    const TextureFormat textureFormat = getFormat(image.channels, image.sRGB, image.isFloat);

    Texture2DSPtr texture = std::make_shared<Texture2D>(
        image.width, image.height, textureFormat, sampler);

    if (!m_api->allocate(texture, image.data))
    {
        texture.reset();
        //TODO log error!
    }

    stbi_image_free(image.data);

    return texture;
}

Texture2DSPtr TextureImporter::importFromFileAsync(
    const std::string& filePath,
    ImportFormat format,
    TextureSampler sampler)
{
    const std::filesystem::path path(filePath);
    if (!std::filesystem::exists(path) || !path.has_filename())
    {
        throw std::runtime_error("Invalid path to Texture file.");
    }

    AsyncTask& task = m_runningTasks.emplace_back();
    // dummy texture
    task.texture = std::make_shared<Texture2D>(1, 1, TextureFormat::RGB, sampler);
    // trigger async texture load
    task.result = std::async(&TextureImporter::loadRawImageFile, this, filePath, format);

    return task.texture;
}

void TextureImporter::waitForCompletion()
{
    for (AsyncTask& task : m_runningTasks)
    {
        ImageWrapper image = task.result.get();
        if (!image.data)
        {
            continue;
        }

        const TextureFormat textureFormat = getFormat(image.channels, image.sRGB, image.isFloat);

        task.texture->update(image.width, image.height, textureFormat);
        if (!m_api->allocate(task.texture, image.data))
        {
            task.texture.reset();
        }

        stbi_image_free(image.data);
    }

    m_runningTasks.clear();
}

TextureImporter::ImageWrapper TextureImporter::loadRawImageFile(const std::string& filepath, ImportFormat format) const
{
    ImageWrapper image = ImageWrapper();

    const std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path) || !path.has_filename())
    {
        return image;
    }

    const auto [desiredChannels, sRGB] = getChannelCountAndColorspace(format);

    image.isFloat = path.extension().string() == ".hdr";
    image.sRGB = !image.isFloat && sRGB;

    stbi_set_flip_vertically_on_load(true);

    if (image.isFloat)
    {
        image.data = stbi_loadf(filepath.c_str(), &image.width, &image.height, &image.channels, desiredChannels);
    }
    else
    {
        image.data = stbi_load(filepath.c_str(), &image.width, &image.height, &image.channels, desiredChannels);
    }

    image.channels = desiredChannels != 0 ? desiredChannels : image.channels;

    return image;
}
