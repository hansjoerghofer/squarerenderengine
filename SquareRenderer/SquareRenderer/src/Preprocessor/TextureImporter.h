#pragma once

#include "Common/Macros.h"
#include "Texture/TextureDefines.h"

#include <string>
#include <future>

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

    ~TextureImporter();

    Texture2DSPtr importFromFile(
        const std::string& filePath,
        ImportFormat format = ImportFormat::Auto,
        TextureSampler sampler = TextureSampler());

    Texture2DSPtr importFromFileAsync(
        const std::string& filePath,
        ImportFormat format = ImportFormat::Auto,
        TextureSampler sampler = TextureSampler());

    void waitForCompletion();

private:

    struct ImageWrapper
    {
        int width;
        int height;
        int channels;
        bool isFloat;
        bool sRGB;
        void* data;
    };

    struct AsyncTask
    {
        Texture2DSPtr texture;
        std::future<ImageWrapper> result;
    };

    std::vector<AsyncTask> m_runningTasks;

    GraphicsAPISPtr m_api;

    ImageWrapper loadRawImageFile(const std::string& path, ImportFormat format = ImportFormat::Auto) const;

};

