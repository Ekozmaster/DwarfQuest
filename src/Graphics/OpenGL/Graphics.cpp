#ifdef _WIN32
#include<Windows.h>
#endif
#include <GL/glew.h>

#include "Graphics.h"
#include "../../Utils/Logger.h"

bool Graphics::Init(SDL_Window *attachedWindow) {
    m_window = attachedWindow;
    bool success = true;

    m_gContext = SDL_GL_CreateContext(m_window);
    if (m_gContext == NULL) {
        Logger::Error("Graphics.Init: OpenGL context could not be created! SDL Error: " + std::string(SDL_GetError()));
        success = false;
    } else if (SDL_GL_SetSwapInterval(1) < 0) {
        Logger::Error("Graphics.Init: Error: Unable to set VSync! SDL Error: " + std::string(SDL_GetError()));
        success = false;
    } else if (glewInit() != GLEW_OK) {
        Logger::Error("Graphics.Init: Error: Unable to set VSync! SDL Error: " + std::string(SDL_GetError()));
        success = false;
    } else {
        SetViewport(640, 480);
    }
    return success;
}

void Graphics::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, 0.5f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.5f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glEnd();
}

void Graphics::Destroy() {
    if(m_gContext) SDL_GL_DeleteContext(m_gContext);
    m_gContext = NULL;
}

void Graphics::SetViewport(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 640, 480);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::SetCameraView(const GLfloat* lookMatrix, const GLfloat* perspectiveMatrix) {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(perspectiveMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(lookMatrix);
}

SDL_Window* Graphics::m_window = nullptr;
SDL_GLContext Graphics::m_gContext = nullptr;