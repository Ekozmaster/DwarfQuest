#ifdef _WIN32
#include<Windows.h>
#endif
#include <glad/glad.h>

#include "Graphics.h"
#include "../../Utils/Logger.h"
#include "ShadersDefinitions.h"
#include "Utils.h"

namespace DwarfQuest {
    namespace Core {

        bool Graphics::Init(SDL_Window* attachedWindow) {
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
            GLTrackCall(glEnable(GL_DEPTH_TEST));

            // Initializing texture slots array.
            GLTrackCall(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTexturesSlots));
            m_activeTextures = new Texture * [m_maxTexturesSlots];
            for (unsigned int i = 0; i < m_maxTexturesSlots; i++) m_activeTextures[i] = NULL;
            return true;
        }

        void Graphics::Destroy() {
            if (m_gContext) SDL_GL_DeleteContext(m_gContext);
            m_gContext = NULL;
            delete[] m_activeTextures;
            m_activeTextures = NULL;
        }

        void Graphics::SetViewport(int width, int height) {
            GLTrackCall(glViewport(0, 0, 640, 480));
        }

        void Graphics::SetShader(Shader* shader) {
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

        void Graphics::SetMesh(Mesh* mesh) {
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
            m_currentMesh->Draw();
        }

        void Graphics::SetTexture(Texture* texture, unsigned int slot) {
            if (slot >= m_maxTexturesSlots) {
                Logger::Error("Graphics.SetTexture - Trying to assign texture to a slot bigger than GL_MAX_TEXTURE_IMAGE_UNITS");
                return;
            }
            m_activeTextures[slot] = texture;
            m_activeTextures[slot]->Use(slot);
        }

        void Graphics::ClearScreen() {
            GLTrackCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
            GLTrackCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        }

        SDL_Window* Graphics::m_window = nullptr;
        SDL_GLContext Graphics::m_gContext = nullptr;
        Shader* Graphics::m_currentShader = nullptr;
        Mesh* Graphics::m_currentMesh = nullptr;
        Texture** Graphics::m_activeTextures = nullptr;
        GLint Graphics::m_maxTexturesSlots = 0;

    }
}