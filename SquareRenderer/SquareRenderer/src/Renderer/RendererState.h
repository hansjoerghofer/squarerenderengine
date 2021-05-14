#pragma once

#include <glm/glm.hpp>

enum class DepthTest
{
    None,
    Always,
    Less,
    LessEqual,
    Equal,
    Greater,
    GreaterEqual,
    NotEqual,
    Never
};

enum class PrimitiveMode
{
    Points,
    Lines,
    Triangles
};

enum class Culling
{
    None,
    Back,
    Front,
    Both
};

enum class BlendFactor
{
    None,
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    DstColor,
    InvDstColor,
    SrcAlpha,
    InvSrcAlpha,
    DstAlpha,
    InvDstAlpha
};

struct RendererState
{
    // clear flags
    bool clearColor = true;
    bool clearDepth = true;
    bool clearStencil = false;

    // clear color
    glm::vec4 color = glm::vec4(0, 0, 0, 1);
    float depth = 1.f;

    // write mask
    bool writeColor = true;
    bool writeDepth = true;

    // render modes
    bool enableWireframe = false;
    DepthTest depthTestMode = DepthTest::Less;
    Culling cullingMode = Culling::Back;
    BlendFactor blendSrc = BlendFactor::None;
    BlendFactor blendDst = BlendFactor::None;
    PrimitiveMode primitive = PrimitiveMode::Triangles;

    // shadow bias
    glm::vec2 depthOffset = glm::vec2(0, 0);

    bool seamlessCubemapFiltering = false;
};