#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace Core {

    typedef struct {
        GLenum type;
        std::string name;
    } ShaderAttribute;

    typedef struct {
        GLenum type;
        std::string name;
        void* dataLocation;
        size_t dataSize;
    } ShaderUniform;

    class Shader {
    private:
        bool m_allocated;
        GLuint m_shaderProgram;

    public:
        Shader();
        ~Shader();

        bool CompileShaders(const char* shaderName, const char* vertexSource, const char* fragmentSource);
        std::vector<ShaderAttribute> GetShaderAttributes();
        std::vector<ShaderUniform> GetShaderUniforms();

        void SetFloatUniform(const char* uniformName, const GLfloat value);
        void SetVec2Uniform(const char* uniformName, const GLfloat* values);
        void SetIVec2Uniform(const char* uniformName, const GLint* values);
        void SetVec3Uniform(const char* uniformName, const GLfloat* values);
        void SetIVec3Uniform(const char* uniformName, const GLint* values);
        void SetVec4Uniform(const char* uniformName, const GLfloat* values);
        void SetIVec4Uniform(const char* uniformName, const GLint* values);
        void SetIntUniform(const char* uniformName, const GLint value);
        void SetUintUniform(const char* uniformName, const GLuint value);
        void SetBoolUniform(const char* uniformName, const GLboolean value);
        void Set2x2MatrixUniform(const char* uniformName, const GLfloat* values);
        void Set3x3MatrixUniform(const char* uniformName, const GLfloat* values);
        void Set4x4MatrixUniform(const char* uniformName, const GLfloat* values);

        void Destroy();

        // Don't use these methods directly unless you know exactly what you're doing.
        int Use();
    };
}
