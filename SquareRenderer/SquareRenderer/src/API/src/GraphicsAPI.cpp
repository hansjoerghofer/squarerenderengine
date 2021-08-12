#include "Common/Logger.h"

#include "API/GraphicsAPI.h"
#include "API/SharedResource.h"
#include "Scene/Geometry.h"
#include "Material/ShaderSource.h"
#include "Material/ShaderProgram.h"
#include "Texture/Texture2D.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/DepthBuffer.h"
#include "Renderer/UniformBlockData.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO check if it must be included adter GLFW?
#include "Common/Math3D.h"

struct GLTextureFormat
{
    GLint internalFormat = 0;
    GLint dataFormat = 0;
    GLint dataType = 0;
    size_t internalBPC = 0;
};

constexpr size_t CHAR_SIZE = 1;
constexpr size_t HALF_SIZE = 2;
constexpr size_t FLOAT_SIZE = 4;

static const std::unordered_map<TextureFormat, GLTextureFormat> s_texFormatToGL =
{
    { TextureFormat::R,             { GL_R8,            GL_RED,     GL_UNSIGNED_BYTE,   CHAR_SIZE } },
    { TextureFormat::RHalf,         { GL_R16F,          GL_RED,     GL_FLOAT,           HALF_SIZE } },
    { TextureFormat::RFloat,        { GL_R32F,          GL_RED,     GL_FLOAT,           FLOAT_SIZE } },
    { TextureFormat::RG,            { GL_RG8,           GL_RG,      GL_UNSIGNED_BYTE,   CHAR_SIZE * 2 } },
    { TextureFormat::RGHalf,        { GL_RG16F,         GL_RG,      GL_FLOAT,           HALF_SIZE * 2 } },
    { TextureFormat::RGFloat,       { GL_RG32F,         GL_RG,      GL_FLOAT,           FLOAT_SIZE * 2 } },
    { TextureFormat::RGB,           { GL_RGB8,          GL_RGB,     GL_UNSIGNED_BYTE,   CHAR_SIZE * 3 } },
    { TextureFormat::RGBHalf,       { GL_RGB16F,        GL_RGB,     GL_FLOAT,           HALF_SIZE * 3 } },
    { TextureFormat::RGBFloat,      { GL_RGB32F,        GL_RGB,     GL_FLOAT,           FLOAT_SIZE * 3 } },
    { TextureFormat::RGBA,          { GL_RGBA8,         GL_RGBA,    GL_UNSIGNED_BYTE,   CHAR_SIZE * 4 } },
    { TextureFormat::RGBAHalf,      { GL_RGBA16F,       GL_RGBA,    GL_FLOAT,           HALF_SIZE * 4 } },
    { TextureFormat::RGBAFloat,     { GL_RGBA32F,       GL_RGBA,    GL_FLOAT,           FLOAT_SIZE * 4 } },
    { TextureFormat::SRGB,          { GL_SRGB8,         GL_RGBA,    GL_UNSIGNED_BYTE,   CHAR_SIZE * 3 } },
    { TextureFormat::SRGBA,         { GL_SRGB8_ALPHA8,  GL_RGBA,    GL_UNSIGNED_BYTE,   CHAR_SIZE * 4 } },
    { TextureFormat::DepthHalf,     { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, FLOAT_SIZE } },
    { TextureFormat::ShadowMapHalf, { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, FLOAT_SIZE } },
    { TextureFormat::DepthFloat,    { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, FLOAT_SIZE } },
    { TextureFormat::ShadowMapFloat,{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, FLOAT_SIZE } }
};

class GLVertexArray : public IGeometryResource
{
public:
    GLVertexArray(GeometrySPtr geometry)
    {
        GLuint VAO, VBO, EBO;

        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            geometry->vertexBufferSize(),
            geometry->vertices().data(),
            geometry->isStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

        if (geometry->indexCount() > 0)
        {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                geometry->indexBufferSize(),
                geometry->indices().data(),
                geometry->isStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        }

        const GLsizei stride = sizeof(Vertex);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,
            GL_FLOAT, GL_FALSE,
            stride, (void*)offsetof(Vertex, position));

        int location = 1;
        if (geometry->hasUVs())
        {
            // vertex texture coords
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 2,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, uv));

            ++location;
        }
        
        if (geometry->hasNormals())
        {
            // vertex normals
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 3,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, normal));

            ++location;
        }

        if (geometry->hasTangents())
        {
            // vertex tangents
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 3,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, tangent));

            ++location;
        }
        glBindVertexArray(0);

        if (GraphicsAPICheckError())
        {
            m_handle = static_cast<SharedResource::Handle>(VAO);
        }
        else
        {
            glDeleteVertexArrays(1, &VAO);
        }
    }

    virtual void bind() override
    {
        if (isValid())
        {
            glBindVertexArray(handle());
        }
    }

    virtual void unbind() override
    {
        glBindVertexArray(0);
    }

    virtual ~GLVertexArray()
    {
        if (isValid())
        {
            const GLuint handle = static_cast<GLuint>(m_handle);
            glDeleteVertexArrays(1, &handle);
            m_handle = INVALID_HANDLE;
        }
    }
};

class GLPositionBuffer : public IGeometryResource
{
    GLPositionBuffer(const std::vector<glm::vec3>& positions)
    {
        GLuint handle;

        glGenBuffers(1, &handle);
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        glBufferData(GL_ARRAY_BUFFER, 
            sizeof(glm::vec3) * positions.size(),
            positions.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (GraphicsAPICheckError())
        {
            m_handle = static_cast<SharedResource::Handle>(handle);
        }
        else
        {
            glDeleteVertexArrays(1, &handle);
        }
    }

    virtual void bind() override
    {
        if (isValid())
        {
           glBindBuffer(GL_ARRAY_BUFFER, m_handle);
        }
    }

    virtual void unbind() override
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    virtual ~GLPositionBuffer()
    {
        if (isValid())
        {
            const GLuint handle = static_cast<GLuint>(m_handle);
            glDeleteBuffers(1, &handle);
            m_handle = INVALID_HANDLE;
        }
    }
};

class GLShader : public IShaderResource
{
public:
    GLShader(ShaderSourceSPtr shader)
    {
        GLenum shaderType = 0;
        switch (shader->type())
        {
        case ShaderType::Vertex:
            shaderType = GL_VERTEX_SHADER;
            break;
        case ShaderType::Fragment:
            shaderType = GL_FRAGMENT_SHADER;
            break;
        case ShaderType::Geometry:
            shaderType = GL_GEOMETRY_SHADER;
            break;
        case ShaderType::Compute:
            shaderType = GL_COMPUTE_SHADER;
            break;
        }

        if (shaderType != 0)
        {
            const char* src = shader->source().c_str();
            const unsigned int handle = glCreateShader(shaderType);
            glShaderSource(handle, 1, &src, NULL);
            glCompileShader(handle);

            int success;
            glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                char infoLog[512];
                glGetShaderInfoLog(handle, 512, NULL, infoLog);

                m_compilerLog = std::string(infoLog);

                glDeleteShader(handle);
            }
            else
            {
                m_handle = static_cast<SharedResource::Handle>(handle);
            }
        }

        GraphicsAPICheckError();
    }

    const std::string& compilerLog() const
    {
        return m_compilerLog;
    }

    virtual ~GLShader()
    {
        if (isValid())
        {
            glDeleteShader(m_handle);
        }
    }

private:
    std::string m_compilerLog;
};

class GLShaderProgram : public IShaderProgramResource
{
public:
    GLShaderProgram(ShaderProgramSPtr program)
        : m_isBound(false)
    {
        unsigned int id = glCreateProgram();

        for (ShaderSourceSPtr shader : program->sources())
        {
            if (shader->id() >= 0)
            {
                glAttachShader(id, shader->id());
            }
        }
        glLinkProgram(id);

        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(id, 512, NULL, infoLog);

            m_linkerLog = std::string(infoLog);
        }
        else
        {
            m_handle = static_cast<SharedResource::Handle>(id);
        }

        GraphicsAPICheckError();
    }

    void bind() override
    {
        if (isValid())
        {
            glUseProgram(static_cast<GLuint>(handle()));
            m_isBound = true;
        }
    }

    void unbind() override
    {
        glUseProgram(0);
        m_isBound = false;
    }

    int uniformLocation(const std::string& name) const override
    {
        if (isValid())
        {
            return glGetUniformLocation(handle(), name.c_str());
        }

        return -1;
    }

    bool bindUniformBlock(const std::string& name, int binding) override
    {
        if (isValid())
        {
            unsigned int idx = glGetUniformBlockIndex(handle(), name.c_str());
            if (idx != GL_INVALID_INDEX)
            {
                glUniformBlockBinding(handle(), idx, binding);

                return GraphicsAPICheckError();
            }
        }   

        return false;
    }

    void setUniform(int location, int value) override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform1i(location, value);
        }
    }

    void setUniform(int location, float value) override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform1f(location, value);
        }
    }

    void setUniform(int location, const glm::vec4& value) override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
    }

    void setUniform(int location, const glm::mat4& value) override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    const std::string& linkerLog() const
    {
        return m_linkerLog;
    }
private:
    std::string m_linkerLog;

    mutable bool m_isBound;
};

class GLUniformBuffer : public IUniformBlockResource
{
public:
    GLUniformBuffer(int location, size_t size)
        : m_bindingPoint(location)
        , m_size(size)
    {
        GLuint handle;
        glGenBuffers(1, &handle);

        glBindBuffer(GL_UNIFORM_BUFFER, handle);
        glBufferData(GL_UNIFORM_BUFFER, m_size, NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, handle);

        if (GraphicsAPICheckError())
        {
            m_handle = static_cast<Handle>(handle);
        }
    }

    int bindingPoint() const override
    {
        return m_bindingPoint;
    }

    bool update(const char* data) override
    {
        if (isValid())
        {
            glBindBuffer(GL_UNIFORM_BUFFER, handle());
            glBufferSubData(GL_UNIFORM_BUFFER, NULL, m_size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            return GraphicsAPICheckError();
        }

        return false;
    }

    ~GLUniformBuffer()
    {
        if (isValid())
        {
            const GLuint handle = static_cast<GLuint>(m_handle);
            glDeleteBuffers(1, &handle);
            m_handle = INVALID_HANDLE;
        }
    }
private:

    Handle m_bindingPoint;
    size_t m_size;
};

class GLTexture2DResource : public ITextureResource
{
public:

    GLTexture2DResource(Texture2DSPtr texture, const void* data = nullptr)
    {
        const auto& found = s_texFormatToGL.find(texture->format());
        if (found == s_texFormatToGL.end())
        {
            return;
        }
        m_format = found->second;

        GLuint textureWrap;
        switch (texture->sampler().wrap)
        {
        case TextureWrap::Mirror:
            textureWrap = GL_MIRRORED_REPEAT;
            break;
        case TextureWrap::ClampToBorder:
            textureWrap = GL_CLAMP_TO_BORDER;
            break;
        case TextureWrap::ClampToEdge:
            textureWrap = GL_CLAMP_TO_EDGE;
            break;
        case TextureWrap::Repeat:
        default:
            textureWrap = GL_REPEAT;
            break;
        }

        GLuint textureMinFilter;
        GLuint textureMaxFilter;
        switch (texture->sampler().filter)
        {
        case TextureFilter::Nearest:
            textureMinFilter = GL_NEAREST_MIPMAP_NEAREST;
            textureMaxFilter = GL_NEAREST;
            break;
        case TextureFilter::Linear:
        default:
            textureMinFilter = GL_LINEAR_MIPMAP_LINEAR;
            textureMaxFilter = GL_LINEAR;
            break;
        }

        if (!texture->sampler().mipmapping)
        {
            textureMinFilter = textureMaxFilter;
        }

        unsigned int handle;
        glGenTextures(1, &handle);
        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureMaxFilter);

        // Enable hardware shadow anti aliasing
        if (texture->format() == TextureFormat::ShadowMapHalf || 
            texture->format() == TextureFormat::ShadowMapFloat)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        }

        if (texture->sampler().wrap == TextureWrap::ClampToBorder)
        {
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, 
                glm::value_ptr(texture->sampler().borderColor));
        }

        update(texture->width(), texture->height(), 
            data, texture->sampler().mipmapping);

        if (GraphicsAPICheckError())
        {
            m_handle = static_cast<SharedResource::Handle>(handle);
        }
        else
        {
            glDeleteTextures(1, &handle);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~GLTexture2DResource()
    {
        if (isValid())
        {
            const GLuint handle = static_cast<GLuint>(m_handle);
            glDeleteTextures(1, &handle);
            m_handle = INVALID_HANDLE;
        }
    }

    void update(int width, int height, const void* data, bool mipmapping) override
    {
        // TODO use glTexSubImage2D if dimensions stay the same and only data changes

        glTexImage2D(GL_TEXTURE_2D, 0, m_format.internalFormat,
            width, height, 0, m_format.dataFormat, m_format.dataType, 
            data);

        if (mipmapping)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    void bind() override
    {
        glBindTexture(GL_TEXTURE_2D, m_handle);
    }

    void unbind() override
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:

    GLTextureFormat m_format;

};

class GLFramebuffer : public IRenderTargetResource
{
public:
    GLFramebuffer(RenderTargetSPtr rendertarget)
    {
        unsigned int handle;
        glGenFramebuffers(1, &handle);
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
        
        GLenum attachement = GL_COLOR_ATTACHMENT0;
        for (ITextureSPtr att : rendertarget->colorTargets())
        {
            switch (att->layout())
            {
            case TextureLayout::Texture2D:
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                    attachement, GL_TEXTURE_2D,
                    att->handle(), 0);
                break;
            /*case TextureLayout::Cubemap:
                glFramebufferTexture(GL_FRAMEBUFFER,
                    attachement, att->handle(),
                    0);
                break;*/
            }

            ++attachement;
        }

        if (rendertarget->depthBuffer())
        {
            GLenum depthFormat = GL_DEPTH_ATTACHMENT;
            if (rendertarget->depthBuffer()->hasStencilBuffer())
            {
                depthFormat = GL_DEPTH_STENCIL_ATTACHMENT;
            }

            if (rendertarget->depthBuffer()->attachmentType() == DepthAttachmentType::Texture2D)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                    depthFormat, GL_TEXTURE_2D,
                    rendertarget->depthBuffer()->handle(), 0);
            }
            else
            {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                    depthFormat, GL_RENDERBUFFER,
                    rendertarget->depthBuffer()->handle());
            }
        }

        if (rendertarget->colorTargets().empty())
        {
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        {
            m_handle = static_cast<SharedResource::Handle>(handle);
        }
        else
        {
            glDeleteFramebuffers(1, &handle);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~GLFramebuffer()
    {
        if (isValid())
        {
            const GLuint handle = static_cast<GLuint>(m_handle);
            glDeleteFramebuffers(1, &handle);
            m_handle = INVALID_HANDLE;
        }
    }
};

class GLRenderbuffer : public IDepthBufferResource
{
public:

    GLRenderbuffer(DepthBufferSPtr depthBuffer)
    {
        switch (depthBuffer->format())
        {
        case DepthBufferFormat::Depth16:
            m_format = GL_DEPTH_COMPONENT16;
            break;
        case DepthBufferFormat::Depth24:
            m_format = GL_DEPTH_COMPONENT24;
            break;
        case DepthBufferFormat::Depth24Stencil8:
            m_format = GL_DEPTH24_STENCIL8;
            break;
        case DepthBufferFormat::DepthFloat:
            m_format = GL_DEPTH_COMPONENT32F;
            break;
        case DepthBufferFormat::DepthFloatStencil8:
            m_format = GL_DEPTH32F_STENCIL8;
            break;
        }

        m_width = depthBuffer->width();
        m_height = depthBuffer->height();

        unsigned int handle;
        glGenRenderbuffers(1, &handle);
        glBindRenderbuffer(GL_RENDERBUFFER, handle);

        glRenderbufferStorage(GL_RENDERBUFFER, m_format,
            depthBuffer->width(), depthBuffer->height());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        m_handle = static_cast<SharedResource::Handle>(handle);
    }

    ~GLRenderbuffer()
    {
        if (isValid())
        {
            const GLuint handle = static_cast<GLuint>(m_handle);
            glDeleteRenderbuffers(1, &handle);
            m_handle = INVALID_HANDLE;
        }
    }
private:

    int m_width = 0;
    int m_height = 0;
    GLuint m_format = 0;
};

GraphicsAPI::GraphicsAPI()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GraphicsAPI::~GraphicsAPI()
{
    glfwTerminate();
}

bool GraphicsAPI::allocate(GeometrySPtr geometry)
{
    if (geometry->linked())
    {
        return true;
    }

    Logger::Info("Allocate geometry buffer (v:%i, f:%i): %.3fKB", 
        geometry->vertexCount(), geometry->indexCount() / 3, 
        (geometry->vertexBufferSize() + geometry->indexBufferSize()) / 1024.f);

    std::unique_ptr<GLVertexArray> resource(new GLVertexArray(geometry));

    // TODO check glError

    if (resource && resource->isValid())
    {
        geometry->link(std::move(resource));
        return true;
    }

    Logger::Error("Could not allocate Geometry.");
    
	return false;
}

bool GraphicsAPI::allocate(ITextureSPtr texture, const void* data)
{
    if (texture->handle() != SharedResource::INVALID_HANDLE)
    {
        return true;
    }

    if (texture->layout() == TextureLayout::Texture2D)
    {
        Texture2DSPtr texture2D = std::static_pointer_cast<Texture2D>(texture);

        const auto& found = s_texFormatToGL.find(texture2D->format());
        if (found == s_texFormatToGL.end())
        {
            return false;
        }

        Logger::Info("Allocate texture (w:%i, h:%i): %.3fKB",
            texture2D->width(), texture2D->height(),
            (static_cast<size_t>(texture2D->width() * texture2D->height()) * found->second.internalBPC) / 1024.f);

        ITextureResourceUPtr resource(new GLTexture2DResource(texture2D, data));

        if (resource && resource->isValid())
        {
            texture2D->link(std::move(resource));
            return true;
        }        
    }

    // TODO other texture layouts

    Logger::Error("Could not allocate Texture.");

    return false;
}

bool GraphicsAPI::allocate(RenderTargetSPtr rendertarget)
{
    if (rendertarget->colorTargets().empty() && 
        !rendertarget->depthBuffer())
    {
        Logger::Error("Could not allocate Render Target,"
                      " no color targets or depth buffer attached.");
        return false;
    }

    for (ITextureSPtr texture : rendertarget->colorTargets())
    {
        if (!allocate(texture))
        {
            Logger::Error("Could not allocate Render Target,"
                          " invalid color target.");
            return false;
        }
    }

    if (rendertarget->depthBuffer() &&
        rendertarget->depthBuffer()->handle() == SharedResource::INVALID_HANDLE)
    {
        if (rendertarget->depthBuffer()->attachmentType() == DepthAttachmentType::Renderbuffer)
        {
            DepthBufferSPtr depthBuffer = std::static_pointer_cast<DepthBuffer>(
                rendertarget->depthBuffer());
            IDepthBufferResourceUPtr depthResource(new GLRenderbuffer(depthBuffer));

            if (depthResource && depthResource->isValid())
            {
                depthBuffer->link(std::move(depthResource));
            }
            else
            {
                Logger::Error("Could not allocate Render Target,"
                    " invalid depth buffer.");
                return false;
            }
        }
        else
        {
            DepthTextureWrapperSPtr depthBuffer = std::static_pointer_cast<DepthTextureWrapper>(
                rendertarget->depthBuffer());
            if (!allocate(depthBuffer->texture()))
            {
                Logger::Error("Could not allocate Render Target,"
                    " invalid color target.");
                return false;
            }
        }
        
    }

    IRenderTargetResourceUPtr resource(new GLFramebuffer(rendertarget));

    if (resource && resource->isValid())
    {
        rendertarget->link(std::move(resource));
        return true;
    }

    Logger::Error("Could not allocate Render Target.");

    return false;
}

bool GraphicsAPI::allocate(IUniformBlockDataSPtr uniformBlockData)
{
    if (uniformBlockData->linked())
    {
        return true;
    }

    IUniformBlockResourceUPtr resource(new GLUniformBuffer(
        uniformBlockData->bindingPoint(), uniformBlockData->dataSize()));

    if (resource && resource->isValid())
    {
        uniformBlockData->link(std::move(resource));
        return true;
    }

    Logger::Error("Could not allocate Uniform block.");

    return false;
}

GraphicsAPI::Result GraphicsAPI::compile(ShaderSourceSPtr shader)
{
    Result result = { true, std::string() };
    if (shader->id() >= 0)
    {
        return result;
    }

    std::unique_ptr<GLShader> resource(new GLShader(shader));

    result.success = resource->isValid();
    result.message = resource->compilerLog();

    if (result)
    {
        shader->link(std::move(resource));
    }
    return result;
}

GraphicsAPI::Result GraphicsAPI::compile(ShaderProgramSPtr program)
{
    // check if shader program is already linked
    Result result = { true, std::string() };
    if (program->id() >= 0)
    {
        return result;
    }

    Logger::Info("Link shader program '%s'", program->name().c_str());

    // compile all the shaders in the program
    unsigned int shaderbits = 0;
    for (const auto& shader : program->sources())
    {
        Result compileResult = compile(shader);
        if (!compileResult)
        {
            return compileResult;
        }

        shaderbits |= 1 << static_cast<unsigned int>(shader->type());
    }

    // check if the shader program consists of a valid combination of shaders
    static const std::vector<unsigned int> valid_combinations =
    {
        1 << static_cast<unsigned int>(ShaderType::Vertex) |
        1 << static_cast<unsigned int>(ShaderType::Fragment),

        1 << static_cast<unsigned int>(ShaderType::Vertex) |
        1 << static_cast<unsigned int>(ShaderType::Geometry) |
        1 << static_cast<unsigned int>(ShaderType::Fragment),

        1 << static_cast<unsigned int>(ShaderType::Compute)
    };

    bool validShaderCombination = false;
    for (unsigned int bitset : valid_combinations)
    {
        if ((shaderbits & bitset) == bitset)
        {
            validShaderCombination = true;
        }
    }
    if (!validShaderCombination)
    {
        return { false, "Invalid shader combination." };
    }

    // link the shader program
    std::unique_ptr<GLShaderProgram> resource(new GLShaderProgram(program));

    result.success = resource->isValid();
    result.message = resource->linkerLog();

    if (result)
    {
        program->link(std::move(resource));
    }

    return result;
}

GraphicsAPISPtr GraphicsAPI::create()
{
    return std::make_shared<GraphicsAPI>();
}

bool GraphicsAPI::checkError(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                               error = "UNHANDLED_ERROR"; break;
        }

        Logger::Error("OpenGL %s | '%s' (ln %i)", error, file, line);
    }
    return errorCode == GL_NO_ERROR;
}
