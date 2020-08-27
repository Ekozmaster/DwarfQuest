#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace DwarfQuest {
    namespace Core {

        typedef struct {
            GLenum type;
            std::string name;
        } ShaderAttribute;

        typedef struct {
            GLenum type;
            std::string name;
        } ShaderUniform;

        class Shader {
        private:
            bool m_allocated;
            GLuint m_shaderProgram;

        public:
            Shader();
            ~Shader();

            bool CompileShaders(const char* shaderName, const char* vertexSource, const char* fragmentSource);
            void Set4x4Matrix(const char* matrixName, const GLfloat* values);
            std::vector<ShaderAttribute> GetShaderAttributes();
            std::vector<ShaderUniform> GetShaderUniforms();
            void Destroy();

            // Don't use these methods directly unless you know exactly what you're doing.
            int Use();
        };
    }
}