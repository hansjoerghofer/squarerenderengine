#include "Renderer/Renderer.h"
#include "API/GraphicsAPI.h"
#include "Application/GL.h"
#include "Common/Logger.h"
#include "Material/Material.h"
#include "Material/ShaderProgram.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/IRenderTarget.h"
#include "Scene/IGeometry.h"
#include "Scene/IGeometryVisitor.h"
#include "Scene/Mesh.h"
#include "Scene/PrimitiveSet.h"
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

inline GLenum translate(PrimitiveType type)
{
    switch (type)
    {
    case PrimitiveType::Points:     return GL_POINTS;
    case PrimitiveType::Lines:      return GL_LINES;
    case PrimitiveType::Triangles:
    default:                        return GL_TRIANGLES;
    }
}

inline GLboolean translate(bool flag)
{
    return flag ? GL_TRUE : GL_FALSE;
}

inline GLenum translate(DrawBuffer drawBuffer)
{
    switch (drawBuffer)
    {
    case DrawBuffer::Attachment0: return GL_COLOR_ATTACHMENT0;
    case DrawBuffer::Attachment1: return GL_COLOR_ATTACHMENT1;
    case DrawBuffer::Attachment2: return GL_COLOR_ATTACHMENT2;
    case DrawBuffer::Attachment3: return GL_COLOR_ATTACHMENT3;
    case DrawBuffer::Attachment4: return GL_COLOR_ATTACHMENT4;
    case DrawBuffer::Attachment5: return GL_COLOR_ATTACHMENT5;
    case DrawBuffer::Attachment6: return GL_COLOR_ATTACHMENT6;
    case DrawBuffer::Attachment7: return GL_COLOR_ATTACHMENT7;
    default: return GL_NONE;
    }
}

void RenderVisitor::prepare(const Material& mat)
{ 
    m_tesselate = mat.program()->supportsTesselation();
}

void RenderVisitor::visit(Mesh& mesh)
{
    const GLsizei indexCount = static_cast<GLsizei>(mesh.indexCount());

    const GLenum mode = m_tesselate ? GL_PATCHES : GL_TRIANGLES;

    glDrawElements(mode, indexCount, GL_UNSIGNED_INT, 0);
}

void RenderVisitor::visit(PrimitiveSet& primitiveSet)
{
    const GLenum type = translate(primitiveSet.type());
    const GLsizei vertexCount = static_cast<GLsizei>(primitiveSet.vertexCount());
    glDrawArrays(type, 0, vertexCount);
}

Renderer::Renderer()
    : m_geometryPainter(new RenderVisitor())
{
    // initalize renderer state
    applyState(RendererState(), true);

    // setup num control vertices for tesselation
    glPatchParameteri(GL_PATCH_VERTICES, 3);
}

void Renderer::render(IGeometrySPtr geo, MaterialSPtr mat)
{
    Logger::Debug("Begin render %s", mat->program()->name().c_str());

    mat->bind();
    bindTextures(mat);
    geo->bind();

    m_geometryPainter->prepare(*mat);

    geo->accept(*m_geometryPainter);

    geo->unbind();
    unbindTextures();
    mat->unbind();

    Logger::Debug("End render %s", mat->program()->name().c_str());
}

void Renderer::blit(IRenderTargetSPtr source, IRenderTargetSPtr target, TextureFilter filter, bool color, bool depth, bool stencil)
{
    GLbitfield blitBits = 0;
    blitBits |= color ? GL_COLOR_BUFFER_BIT : 0;
    blitBits |= depth ? GL_DEPTH_BUFFER_BIT : 0;
    blitBits |= stencil ? GL_STENCIL_BUFFER_BIT : 0;

    if (blitBits == 0)
    {
        return;
    }

    const GLenum filterEnum = filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST;

    glBlitNamedFramebuffer(
        source->handle(), target->handle(),
        0, 0, source->width(), source->height(),
        0, 0, target->width(), target->height(),
        blitBits, filterEnum);

    Logger::Debug("Blit %i -> %i", source->handle(), target->handle());
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

        m_currentRenderTarget = target;
    }

    std::string attachmentsIds = "-";
    if (m_currentRenderTarget->handle() != SharedResource::Handle(0))
    {
        attachmentsIds = "";
        RenderTargetSPtr debT = std::static_pointer_cast<RenderTarget>(m_currentRenderTarget);
        for (ITextureSPtr t : debT->colorTargets())
        {
            attachmentsIds += " " + std::to_string(t->handle());
        }
    }
    Logger::Debug("Bind framebuffer: %i (Textures: %s)", m_currentRenderTarget->handle(), attachmentsIds.c_str());
}

void Renderer::applyState(const RendererState& state, bool force)
{
    // TODO check if it makes more sense as part of IRenderTarget::bind
    if ((force || state.drawBuffers != m_currentState.drawBuffers) && m_currentRenderTarget)
    {
        m_currentState.drawBuffers = state.drawBuffers;

        std::vector<GLenum> drawBuffers;
        drawBuffers.reserve(state.drawBuffers.size());
        for (DrawBuffer e : state.drawBuffers) { drawBuffers.push_back(translate(e)); }

        glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    }

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
    if (clearBits != 0)
    {
        glClear(clearBits);
    }

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
}

void Renderer::regenerateMipmaps(ITextureSPtr tex)
{
    switch (tex->layout())
    {
    case TextureLayout::Texture2D:
        glBindTexture(GL_TEXTURE_2D, tex->handle());
        glGenerateMipmap(GL_TEXTURE_2D);
        break;
    case TextureLayout::Cubemap:
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex->handle());
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
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
        GLenum target;
        switch (texture->layout())
        {
        case TextureLayout::Texture2D:
            target = GL_TEXTURE_2D;
            break;
        case TextureLayout::Cubemap:
            target = GL_TEXTURE_CUBE_MAP;
            break;
        default:
            throw std::exception("Try to bind unsupported texture layout.");
        }

        mat->setUniform(name, activeTextureUnit);
        glActiveTexture(GL_TEXTURE0 + activeTextureUnit);
        glBindTexture(target, texture->handle());

        m_boundTextures.push_back(texture);

        ++activeTextureUnit;

        Logger::Debug("Bind texture %i to '%s'", texture->handle(), name.c_str());
    }
}

void Renderer::unbindTextures()
{
    for (size_t i = 0; i < m_boundTextures.size(); ++i)
    {
        glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + i));
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    m_boundTextures.clear();
}
