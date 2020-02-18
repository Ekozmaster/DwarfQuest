#ifdef _WIN32
#include<Windows.h>
#endif
#include <glad/glad.h>

#include "Graphics.h"
#include "../../Utils/Logger.h"
#include "ShadersDefinitions.h"

bool Graphics::Init(SDL_Window *attachedWindow) {
    m_window = attachedWindow;
    bool success = true;

    // CONTEXT
    m_gContext = SDL_GL_CreateContext(m_window);
    if (m_gContext == NULL) {
        Logger::Error("Graphics.Init: OpenGL context could not be created! SDL Error: " + std::string(SDL_GetError()));
        return false;
    }

    // VSYNC
    if (SDL_GL_SetSwapInterval(1) < 0) {
        Logger::Error("Graphics.Init: Error: Unable to set VSync! SDL Error: " + std::string(SDL_GetError()));
        SDL_GL_DeleteContext(m_gContext);
        return false;
    }

    // GLAD INIT
    int glExtensionsLoaded = gladLoadGL();
    if (!glExtensionsLoaded) {
        Logger::Error("Graphics.Init: Error: Unable to initialize glad");
        SDL_GL_DeleteContext(m_gContext);
        return false;
    }

    SetViewport(640, 480);
    return true;
}

void Graphics::Destroy() {
    if(m_gContext) SDL_GL_DeleteContext(m_gContext);
    m_gContext = NULL;
}

void Graphics::SetViewport(int width, int height) {
    glViewport(0, 0, 640, 480);
}

void Graphics::SetShader(Shader *shader) {
    m_currentShader = shader;
    m_currentShader->Use();
}

void Graphics::SetShaderMatrix(const char* matrixName, const GLfloat* values) {
    if (!m_currentShader) {
        Logger::Error("Graphics.SetShaderMatrices - Trying to bind matrix with no shader attached.");
        return;
    }
    m_currentShader->Set4x4Matrix(matrixName, values);
}

void Graphics::SetMesh(Mesh *mesh) {
    m_currentMesh = mesh;
    m_currentMesh->Use();
}

void Graphics::RenderMesh() {
    if (!m_currentShader) {
        Logger::Error("Graphics.RenderMesh - Trying to render mesh with no shader attached.");
        return;
    }
    if (!m_currentMesh) {
        Logger::Error("Graphics.RenderMesh - Trying to render mesh with no mesh attached.");
        return;
    }
    m_currentMesh->Render();
}

void Graphics::ClearScreen() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

SDL_Window* Graphics::m_window = nullptr;
SDL_GLContext Graphics::m_gContext = nullptr;
Shader* Graphics::m_currentShader = nullptr;
Mesh* Graphics::m_currentMesh = nullptr;