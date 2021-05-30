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

TextureImporter::TextureImporter(GraphicsAPISPtr api)
    : m_api(api)
{
}

Texture2DSPtr TextureImporter::importFromFile(
    const std::string& filePath, 
    ImportFormat format,
    TextureSampler sampler)
{
    int desiredChannels = 0;
    switch (format)
    {
    case ImportFormat::R:
        desiredChannels = 1;
        break;
    case ImportFormat::RG:
        desiredChannels = 2;
        break;
    case ImportFormat::RGB:
    case ImportFormat::sRGB:
        desiredChannels = 3;
        break;
    case ImportFormat::RGBA:
    case ImportFormat::sRGBA:
        desiredChannels = 4;
        break;
    }
    const bool sRGB =   format == ImportFormat::Auto || 
                        format == ImportFormat::sRGB ||
                        format == ImportFormat::sRGBA;

    stbi_set_flip_vertically_on_load(true);

    // TODO async
    ImageWrapper image = loadRawImageFile(filePath, desiredChannels);
    if (!image.data)
    {
        return nullptr;
    }

    const TextureFormat textureFormat = getFormat(image.channels, sRGB, image.isFloat);

    const size_t bpc = image.isFloat ? 4 : 1;
    const size_t dataSize = static_cast<size_t>(image.width * image.height) * image.channels * bpc;

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

TextureImporter::ImageWrapper TextureImporter::loadRawImageFile(const std::string& filepath, int desiredChannels)
{
    const std::filesystem::path path(filepath);

    if (!std::filesystem::exists(path) || !path.has_filename())
    {
        throw std::runtime_error("Invalid path to Texture file.");
    }

    ImageWrapper image = ImageWrapper();
    image.isFloat = path.extension().string() == ".hdr";

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
