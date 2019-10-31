#include "Graphics.h"
#include <SDL_opengl.h>
#include <GL\GLU.h>
#include "../../Utils/Logger.h"

bool Graphics::Init(SDL_Window *attachedWindow) {
    m_window = attachedWindow;
    bool success = true;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    m_gContext = SDL_GL_CreateContext(m_window);
    if (m_gContext == NULL) {
        Logger::Error("Graphics.Init: OpenGL context could not be created! SDL Error: " + std::string(SDL_GetError()));
        success = false;
    } else if (SDL_GL_SetSwapInterval(1) < 0) {
        Logger::Error("Graphics.Init: Warning: Unable to set VSync! SDL Error: " + std::string(SDL_GetError()));
        success = false;
    } else {
        SetViewport(640, 480);
    }
    return success;
}

void Graphics::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

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

SDL_Window* Graphics::m_window = nullptr;
SDL_GLContext Graphics::m_gContext = nullptr;