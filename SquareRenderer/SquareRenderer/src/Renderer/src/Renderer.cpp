#include "API/GraphicsAPI.h"
#include "Renderer/Renderer.h"
#include "Renderer/IRenderTarget.h"
#include "Renderer/IGeometry.h"
#include "Material/Material.h"
#include "Material/ShaderProgram.h"
#include "Texture/ITexture.h"

inline GLenum translate(BlendFactor factor)
{
    switch (factor)
    {
    case BlendFactor::Zero:         return GL_ZERO;
    case BlendFactor::One:          return GL_ONE;
    case BlendFactor::SrcColor:     return GL_SRC_COLOR;
    case BlendFactor::InvSrcColor:  return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DstColor:     return GL_DST_COLOR;
    case BlendFactor::InvDstColor:  return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::SrcAlpha:     return GL_SRC_ALPHA;
    case BlendFactor::InvSrcAlpha:  return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DstAlpha:     return GL_DST_ALPHA;
    case BlendFactor::InvDstAlpha:  return GL_ONE_MINUS_DST_ALPHA;
    default:                        return GL_INVALID_ENUM;
    }
}

inline GLenum translate(DepthTest test)
{
    switch (test)
    {
    case DepthTest::Less:           return GL_LESS;
    case DepthTest::LessEqual:      return GL_LEQUAL;
    case DepthTest::Equal:          return GL_EQUAL;
    case DepthTest::Greater:        return GL_GREATER;
    case DepthTest::GreaterEqual:   return GL_GEQUAL;
    case DepthTest::NotEqual:       return GL_NOTEQUAL;
    case DepthTest::Always:         return GL_ALWAYS;
    case DepthTest::Never:          return GL_NEVER;
    default:                        return GL_INVALID_ENUM;
    }
}

inline GLenum translate(Culling culling)
{
    switch (culling)
    {
    case Culling::Back:     return GL_BACK;
    case Culling::Front:    return GL_FRONT;
    case Culling::Both:     return GL_FRONT_AND_BACK;
    default:                return GL_INVALID_ENUM;
    }
}

inline GLenum translate(PrimitiveMode mode)
{
    switch (mode)
    {
    case PrimitiveMode::Points:     return GL_POINTS;
    case PrimitiveMode::Lines:      return GL_LINES;
    case PrimitiveMode::Triangles: 
    default:                        return GL_TRIANGLES;
    }
}

inline GLboolean translate(bool flag)
{
    return flag ? GL_TRUE : GL_FALSE;
}

Renderer::Renderer()
{
    // initalize renderer state
    applyState(RendererState(), true);
}

void Renderer::render(IGeometrySPtr geo, MaterialSPtr mat)
{
    mat->bind();
    bindTextures(mat);
    geo->bind();

    GLenum primitiveMode = translate(m_currentState.primitive);
    if (geo->indexCount() > 0)
    {
        const GLsizei indexCount = static_cast<GLsizei>(geo->indexCount());
        glDrawElements(primitiveMode, indexCount, GL_UNSIGNED_INT, 0);
    }
    else
    {
        const GLsizei vertexCount = static_cast<GLsizei>(geo->vertexCount());
        glDrawArrays(primitiveMode, 0, vertexCount);
    }

    geo->unbind();
    unbindTextures();
    mat->unbind();

    GraphicsAPICheckError();
}

void Renderer::setTarget(IRenderTargetSPtr target)
{
    if (target && target != m_currentRenderTarget)
    {
        glViewport(
            0,0,
            target->width(),
            target->height()
        );

        GLuint fbo = target->handle();
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        GraphicsAPICheckError();

        m_currentRenderTarget = target;
    }
}

void Renderer::applyState(const RendererState& state, bool force)
{
    if (force || state.enableWireframe != m_currentState.enableWireframe)
    {
        m_currentState.enableWireframe = state.enableWireframe;
        glPolygonMode(GL_FRONT_AND_BACK, state.enableWireframe ? GL_LINE : GL_FILL);
    }

    if (force || state.writeColor != m_currentState.writeColor)
    {
        m_currentState.writeColor = state.writeColor;
        const GLboolean flag = translate(state.writeColor);
        glColorMask(flag, flag, flag, flag);
    }

    if (force || state.writeDepth != m_currentState.writeDepth)
    {
        m_currentState.writeDepth = state.writeDepth;
        const GLboolean flag = translate(state.writeDepth);
        glDepthMask(flag);
    }

    if (force || (state.clearColor && state.color != m_currentState.color))
    {
        m_currentState.color = state.color;
        glClearColor(
            state.color.r,
            state.color.g,
            state.color.b,
            state.color.a);
    }

    if (force || (state.clearDepth && state.depth != m_currentState.depth))
    {
        m_currentState.depth = state.depth;
        glClearDepth(state.depth);
    }

    if (force 
        || state.blendSrc != m_currentState.blendSrc 
        || state.blendDst != m_currentState.blendDst)
    {
        m_currentState.blendSrc = state.blendSrc;
        m_currentState.blendDst = state.blendDst;

        if (state.blendSrc != BlendFactor::None &&
            state.blendDst != BlendFactor::None)
        {
            const GLenum srcFactor = translate(state.blendSrc);
            const GLenum dstFactor = translate(state.blendDst);

            glBlendFunc(srcFactor, dstFactor);
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    if (force || state.depthOffset != m_currentState.depthOffset)
    {
        m_currentState.depthOffset = state.depthOffset;

        if (state.depthOffset.length() > 0)
        {
            glPolygonOffset(
                state.depthOffset.x,
                state.depthOffset.y
            );
            glEnable(GL_POLYGON_OFFSET_FILL);
        }
        else
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

    GLbitfield clearBits = 0;
    clearBits |= state.clearColor ? GL_COLOR_BUFFER_BIT : 0;
    clearBits |= state.clearDepth ? GL_DEPTH_BUFFER_BIT : 0;
    clearBits |= state.clearStencil ? GL_STENCIL_BUFFER_BIT : 0;
    glClear(clearBits);

    m_currentState.clearColor = state.clearColor;
    m_currentState.clearDepth = state.clearDepth;
    m_currentState.clearStencil = state.clearStencil;

    if (force || state.depthTestMode != m_currentState.depthTestMode)
    {
        m_currentState.depthTestMode = state.depthTestMode;

        if (state.depthTestMode != DepthTest::None)
        {
            const GLenum depthFunc = translate(state.depthTestMode);

            glDepthFunc(depthFunc);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }

    if (force || state.cullingMode != m_currentState.cullingMode)
    {
        m_currentState.cullingMode = state.cullingMode;

        if (state.cullingMode != Culling::None)
        {
            const GLenum mode = translate(state.cullingMode);

            glCullFace(mode);
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }

    if (force || state.seamlessCubemapFiltering != m_currentState.seamlessCubemapFiltering)
    {
        m_currentState.seamlessCubemapFiltering = state.seamlessCubemapFiltering;

        if (state.seamlessCubemapFiltering)
        {
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }
        else
        {
            glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }
    }

    m_currentState.primitive = state.primitive;

    GraphicsAPICheckError();
}

void Renderer::bindTextures(MaterialSPtr mat)
{
    auto textures = mat->uniformTextures();
    textures.insert(
        mat->program()->defaultTextures().begin(),
        mat->program()->defaultTextures().end());

    m_boundTextures.reserve(textures.size());

    int activeTextureUnit = 0;
    for (const auto& [name, texture] : textures)
    {
        mat->setUniform(name, activeTextureUnit);
        glActiveTexture(GL_TEXTURE0 + activeTextureUnit);
        texture->bind();

        m_boundTextures.push_back(texture);

        ++activeTextureUnit;
    }
}

void Renderer::unbindTextures()
{
    GLenum activeTexture = GL_TEXTURE0;
    for (auto texture : m_boundTextures)
    {
        glActiveTexture(activeTexture++);
        texture->unbind();
    }

    m_boundTextures.clear();
}
