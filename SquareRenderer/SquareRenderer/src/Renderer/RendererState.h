#pragma once

#include "Common/Math3D.h"
#include <vector>

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

enum class DrawBuffer
{
    Attachment0,
    Attachment1,
    Attachment2,
    Attachment3,
    Attachment4,
    Attachment5,
    Attachment6,
    Attachment7
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

    std::vector<DrawBuffer> drawBuffers = { DrawBuffer::Attachment0 };

    // render modes
    bool enableWireframe = false;
    DepthTest depthTestMode = DepthTest::Less;
    Culling cullingMode = Culling::Back;
    BlendFactor blendSrc = BlendFactor::None;
    BlendFactor blendDst = BlendFactor::None;
    
    // shadow bias
    glm::vec2 depthOffset = glm::vec2(0, 0);

    bool seamlessCubemapFiltering = false;

    static RendererState Blit()
    {
        RendererState state;
        state.clearColor = false;
        state.clearDepth = false;
        state.clearStencil = false;
        state.color = glm::vec4(1, 1, 1, 1);
        state.depthTestMode = DepthTest::None;
        return state;
    }

    static RendererState Add()
    {
        RendererState state = Blit();
        state.clearColor = false;
        state.blendSrc = BlendFactor::One;
        state.blendDst = BlendFactor::One;
        return state;
    }
};