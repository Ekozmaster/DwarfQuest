#pragma once
#ifdef _WIN32
#include<Windows.h>
#endif
#include<SDL2/SDL.h>

#include "Mesh.h"
#include "Shader.h"

class Graphics {
private:
    static Shader* m_currentShader;
    static Mesh* m_currentMesh;
    static SDL_Window* m_window;
    static SDL_GLContext m_gContext;

public:
    static bool Init(SDL_Window* attachedWindow);
    static void Destroy();

    static void SetViewport(int width, int height);
    static void SetShader(Shader *shader);
    static void SetShaderMatrix(const char* matrixName, const GLfloat* values);
    static void SetMesh(Mesh *mesh);
    static void RenderMesh();
    static void ClearScreen();
};