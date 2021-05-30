#pragma once

#include "Common/Macros.h"
#include "Texture/TextureDefines.h"

#include <string>

DECLARE_PTRS(Texture2D);
DECLARE_PTRS(GraphicsAPI);

class TextureImporter
{
public:
    enum class ImportFormat
    {
        Auto, R, RG, RGB, RGBA, sRGB, sRGBA
    };

    TextureImporter(GraphicsAPISPtr api);

    Texture2DSPtr importFromFile(
        const std::string& filePath,
        ImportFormat format = ImportFormat::Auto,
        TextureSampler sampler = TextureSampler());

private:

    struct ImageWrapper
    {
        int width;
        int height;
        int channels;
        bool isFloat;
        void* data;
    };

    GraphicsAPISPtr m_api;

    ImageWrapper loadRawImageFile(const std::string& path, int desiredChannels = 0);

};

