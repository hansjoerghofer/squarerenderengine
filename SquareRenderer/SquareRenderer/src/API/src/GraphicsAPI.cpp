#include "API/GraphicsAPI.h"
#include "API/SharedResource.h"
#include "Scene/Geometry.h"
#include "Material/ShaderSource.h"
#include "Material/ShaderProgram.h"
#include "Material/UniformBlock.h"
#include "Common/Logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

class GLVertexArray : public GeometryResource
{
public:
    GLVertexArray(const Geometry& geometry) : GeometryResource()
    {
        GLuint VAO, VBO, EBO;

        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            geometry.vertexBufferSize(),
            geometry.vertices().data(),
            geometry.isStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

        if (geometry.indexCount() > 0)
        {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                geometry.indexBufferSize(),
                geometry.indices().data(),
                geometry.isStatic() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        }

        const GLsizei stride = sizeof(Vertex);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,
            GL_FLOAT, GL_FALSE,
            stride, (void*)offsetof(Vertex, position));

        int location = 1;
        if (geometry.hasUVs())
        {
            // vertex texture coords
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 2,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, uv));

            ++location;
        }
        
        if (geometry.hasNormals())
        {
            // vertex normals
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 3,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, normal));

            ++location;
        }

        if (geometry.hasTangents())
        {
            // vertex tangents
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 3,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, tangent));

            ++location;
        }
        glBindVertexArray(0);

        GraphicsAPICheckError();

        m_handle = static_cast<SharedResourceHandle>(VAO);
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

class GLShader : public SharedResource
{
public:
    GLShader(const ShaderSource& shader)
    {
        GLenum shaderType = 0;
        switch (shader.type())
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
            const char* src = shader.source().c_str();
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
                m_handle = static_cast<SharedResourceHandle>(handle);
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

class GLShaderProgram : public ShaderProgramResource
{
public:
    GLShaderProgram(const ShaderProgram& program)
        : m_isBound(false)
    {
        unsigned int id = glCreateProgram();

        for (const auto& shader : program.sources())
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
            m_handle = static_cast<SharedResourceHandle>(id);
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

    void setUniform(int location, unsigned int value) override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform1ui(location, value);
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

    void setUniform(int location, const glm::mat4x4& value) override
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

class GLUniformBuffer : public UniformBlockResource
{
public:
    GLUniformBuffer(unsigned int location, size_t size)
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
            m_handle = static_cast<SharedResourceHandle>(handle);
        }
    }

    int bindingPoint() const override
    {
        return m_bindingPoint;
    }

    void update(const char* data) override
    {
        if (isValid())
        {
            glBindBuffer(GL_UNIFORM_BUFFER, handle());
            glBufferSubData(GL_UNIFORM_BUFFER, NULL, m_size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);

            GraphicsAPICheckError();
        }
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

    int m_bindingPoint;
    size_t m_size;
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

bool GraphicsAPI::allocate(Geometry& geometry)
{
    if (geometry.linked())
    {
        return true;
    }

    Logger::Info("Allocate geometry buffer (v:%i, f:%i): %iB", 
        geometry.vertexCount(), geometry.indexCount() / 3, 
        geometry.vertexBufferSize() + geometry.indexBufferSize());

    std::unique_ptr<GLVertexArray> resource(new GLVertexArray(geometry));

    // TODO check glError

    if (resource && resource->isValid())
    {
        geometry.link(std::move(resource));
        return true;
    }
    
	return false;
}

GraphicsAPI::Result GraphicsAPI::compile(ShaderSource& shader)
{
    Result result = { true, std::string() };
    if (shader.id() >= 0)
    {
        return result;
    }

    std::unique_ptr<GLShader> resource(new GLShader(shader));

    result.success = resource->isValid();
    result.message = resource->compilerLog();

    if (result)
    {
        shader.link(std::move(resource));
    }
    return result;
}

GraphicsAPI::Result GraphicsAPI::compile(ShaderProgram& program)
{
    // check if shader program is already linked
    Result result = { true, std::string() };
    if (program.id() >= 0)
    {
        return std::move(result);
    }

    Logger::Info("Link shader program '%s'", program.name().c_str());

    // compile all the shaders in the program
    unsigned int shaderbits = 0;
    for (const auto& shader : program.sources())
    {
        Result compileResult = compile(*shader);
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
        program.link(std::move(resource));
    }

    return result;
}

UniformBlockResourceUPtr GraphicsAPI::allocateUniformBlock(int location, size_t size)
{
    return UniformBlockResourceUPtr(new GLUniformBuffer(location, size));
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
