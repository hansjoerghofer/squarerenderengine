#pragma once

#include <glm/glm.hpp>

enum class TextureLayout
{
    Image,
    Texture2D,
    Cubemap
};

enum class TextureAccess
{
    Read,
    Write,
    ReadWrite
};

enum class TextureFormat
{
    R,
    RG,
    RGB,
    RGBA,
    SRGB,
    SRGBA,

    RHalf,
    RGHalf,
    RGBHalf,
    RGBAHalf,
    DepthHalf,
    ShadowMapHalf,

    RFloat,
    RGFloat,
    RGBFloat,
    RGBAFloat,
    DepthFloat,
    ShadowMapFloat
};

enum class TextureFilter
{
    Nearest,
    Linear
};

enum class TextureWrap
{
    Repeat,
    Mirror,
    ClampToEdge,
    ClampToBorder
};

struct TextureSampler
{
    TextureFilter filter = TextureFilter::Linear;
    TextureWrap wrap = TextureWrap::Repeat;
    bool mipmapping = true;
    glm::vec4 borderColor = glm::vec4(0);
};