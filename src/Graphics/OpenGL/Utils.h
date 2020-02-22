#include <glad/glad.h>
#include <iostream>
#include <string>

void ClearOpenGLErrorQueue();
const std::string GlErrorName(GLuint errorCode);

#define GL_CHECK_ERRORS(x) while (const GLuint error = glGetError()) { std::cout << "OpenGL Error in " << __FILE__ << ", when calling " << x << ", at line " << __LINE__ << ": " << GlErrorName(error) << std::endl; }

#define GLTrackCall(x) ClearOpenGLErrorQueue(); x; GL_CHECK_ERRORS(#x)