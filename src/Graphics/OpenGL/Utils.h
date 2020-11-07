#include <glad/glad.h>
#include <iostream>
#include <string>

namespace DwarfQuest {
    namespace Core {

        inline int GetGLTypeSize(GLenum typeCode) {
            switch (typeCode) {
            case GL_FLOAT:
                return sizeof(GLfloat);
            case GL_FLOAT_VEC2:
                return sizeof(GLfloat) * 2;
            case GL_INT_VEC2:
                return sizeof(GLint) * 2;
            case GL_FLOAT_VEC3:
                return sizeof(GLfloat) * 3;
            case GL_INT_VEC3:
                return sizeof(GLint) * 3;
            case GL_FLOAT_VEC4:
                return sizeof(GLfloat) * 4;
            case GL_INT_VEC4:
                return sizeof(GLint) * 4;
            case GL_INT:
                return sizeof(GLint);
            case GL_UNSIGNED_INT:
                return sizeof(GLuint);
            case GL_BOOL:
                return sizeof(GLboolean);
            case GL_FLOAT_MAT2:
                return sizeof(GLfloat) * 4;
            case GL_FLOAT_MAT3:
                return sizeof(GLfloat) * 9;
            case GL_FLOAT_MAT4:
                return sizeof(GLfloat) * 16;
            case GL_SAMPLER_2D:
                // Pointers to Texture objects.
                return sizeof(void*);
            case GL_SAMPLER_CUBE:
                // Pointers to CubeTexture objects.
                return sizeof(void*);
            }
            return -1;
        }

        void ClearOpenGLErrorQueue();
        const std::string GlErrorName(GLuint errorCode);

#define GL_CHECK_ERRORS(x) while (const GLuint error = glGetError()) { std::cout << "OpenGL Error in " << __FILE__ << ", when calling " << x << ", at line " << __LINE__ << ": " << GlErrorName(error) << std::endl; }

#define GLTrackCall(x) ClearOpenGLErrorQueue(); x; GL_CHECK_ERRORS(#x)

    }
}