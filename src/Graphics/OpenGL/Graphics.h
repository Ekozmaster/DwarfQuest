#pragma once
#ifdef _WIN32
#include<Windows.h>
#endif
#include<SDL2/SDL.h>
#include<vector>
#include<string>
#include<glm/glm.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <src/Graphics/OpenGL/Camera.h>

namespace Core {
    
    class Graphics {
    private:
        static Shader* m_currentShader;
        static Mesh* m_currentMesh;
        static SDL_Window* m_window;
        static SDL_GLContext m_gContext;
        static Texture** m_activeTextures;
        static GLint m_maxTexturesSlots;
        static Camera::Camera* m_currentCamera;
        static glm::ivec2 m_cachedScreenResolution;

    public:
        static std::vector<std::string> graphicsUniformsNames;

        static bool Init(SDL_Window* attachedWindow, unsigned int width, unsigned int height);
        static void Destroy();

        static void SetViewport(int width, int height);
        static void SetShader(Shader* shader);
        static void SetShaderMatrix(const char* matrixName, const GLfloat* values);
        static void SetShaderIVector2(const char* vectorName, const GLint* values);
        static void SetMesh(Mesh* mesh);
        static void RenderMesh();
        static void SetTexture(Texture* texture, unsigned int slot);
        static void ClearScreen();
        static void SetCurrentCamera(Camera::Camera* camera);
    };
}

