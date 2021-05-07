#include "Resources/GLAllocator.h"
#include "Resources/SharedResource.h"
#include "Scene/Geometry.h"
#include "Material/ShaderSource.h"
#include "Material/ShaderProgram.h"

#include <Application/GL.h>

#include <glm/gtc/type_ptr.hpp>

class GLVertexArray : public GeometryResource
{
public:
    GLVertexArray(const Geometry& geometry) : GeometryResource()
    {
        unsigned int VAO, VBO, EBO;

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

        const size_t stride = sizeof(Vertex);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3,
            GL_FLOAT, GL_FALSE,
            stride, (void*)offsetof(Vertex, position));

        // vertex texture coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2,
            GL_FLOAT, GL_FALSE,
            stride, (void*)offsetof(Vertex, uv));

        // TODO
        //if (includeNormTan)
        {
            // vertex normals
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, normal));

            // vertex tangents
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3,
                GL_FLOAT, GL_FALSE,
                stride, (void*)offsetof(Vertex, tangent));
        }
        glBindVertexArray(0);

        m_handle = static_cast<SharedResourceHandle>(VAO);
    }

    virtual ~GLVertexArray()
    {
        if (isValid())
        {
            const GLuint handle = static_cast<GLuint>(m_handle);
            glDeleteVertexArrays(1, &handle);
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

        const auto shaders = program.sources();
        for (const auto& shader : shaders)
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
    }

    void bind() const override
    {
        if (isValid())
        {
            glUseProgram(static_cast<GLuint>(handle()));
            m_isBound = true;
        }
    }

    void unbind() const override
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

    void setUniform(int location, int value) const override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform1i(location, value);
        }
    }

    void setUniform(int location, unsigned int value) const override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform1ui(location, value);
        }
    }

    void setUniform(int location, float value) const override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform1f(location, value);
        }
    }

    void setUniform(int location, const glm::vec4& value) const override
    {
        if (isValid() && m_isBound && location >= 0)
        {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
    }

    void setUniform(int location, const glm::mat4x4& value) const override
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

bool GLAllocator::allocate(Geometry& geometry)
{
    if (geometry.hasLink())
    {
        return true;
    }

    std::unique_ptr<GLVertexArray> resource(new GLVertexArray(geometry));

    // TODO check glError

    if (resource && resource->isValid())
    {
        geometry.link(std::move(resource));
        return true;
    }
    
	return false;
}

GLAllocator::Result GLAllocator::compile(ShaderSource& shader)
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
    return std::move(result);
}

GLAllocator::Result GLAllocator::link(ShaderProgram& program)
{
    // check if shader program is already linked
    Result result = { true, std::string() };
    if (program.id() >= 0)
    {
        return std::move(result);
    }

    // compile all the shaders in the program
    unsigned int shaderbits = 0;
    const auto shaders = program.sources();
    for (const auto& shader : shaders)
    {
        Result compileResult = compile(*shader);
        if (!compileResult)
        {
            return std::move(compileResult);
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

    return std::move(result);
}
