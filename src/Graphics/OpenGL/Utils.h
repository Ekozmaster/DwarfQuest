#include <glad/glad.h>
#include <iostream>
#include <string>

void ClearOpenGLErrorQueue() {
    while (glGetError());
}

const std::string GlErrorName(GLuint errorCode) {
    switch(errorCode) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR (" + std::to_string(errorCode) + ")";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM (" + std::to_string(errorCode) + ")";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE (" + std::to_string(errorCode) + ")";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION (" + std::to_string(errorCode) + ")";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION (" + std::to_string(errorCode) + ")";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY (" + std::to_string(errorCode) + ")";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW (" + std::to_string(errorCode) + ")";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW (" + std::to_string(errorCode) + ")";
    }
    return std::string("Unexpected Error (") + std::to_string(errorCode) + ")";
}

#define GL_CHECK_ERRORS(x) while (const GLuint error = glGetError()) { std::cout << "OpenGL Error in " << __FILE__ << ", when calling " << x << ", at line " << __LINE__ << ": " << GlErrorName(error) << std::endl; }

#define GLTrackCall(x) ClearOpenGLErrorQueue(); x; GL_CHECK_ERRORS(#x)
