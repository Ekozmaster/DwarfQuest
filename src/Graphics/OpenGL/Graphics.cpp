#ifdef _WIN32
#include<Windows.h>
#endif
#include <glad/glad.h>

#include "Graphics.h"
#include "../../Utils/Logger.h"
#include "ShadersDefinitions.h"
#include "Utils.h"
#include <glm/gtc/type_ptr.hpp>

namespace DwarfQuest {
    namespace Core {

        bool Graphics::Init(SDL_Window* attachedWindow, unsigned int width, unsigned int height) {
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

            SetViewport(width, height);
            m_cachedScreenResolution = glm::ivec2(width, height);
            GLTrackCall(glEnable(GL_DEPTH_TEST));

            // Initializing texture slots array.
            GLTrackCall(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTexturesSlots));
            m_activeTextures = new Texture * [m_maxTexturesSlots];
            for (unsigned int i = 0; i < m_maxTexturesSlots; i++) m_activeTextures[i] = NULL;

            // MISC
            // Shader uniforms that Graphics only is responsible for.
            graphicsUniformsNames.push_back(SHADERS_PERSPECTIVE_MATRIX);
            graphicsUniformsNames.push_back(SHADERS_VIEW_MATRIX);
            graphicsUniformsNames.push_back(SHADERS_MODEL_MATRIX);
            graphicsUniformsNames.push_back(SHADERS_SCREEN_RESOLUTION);

            return true;
        }

        void Graphics::Destroy() {
            if (m_gContext) SDL_GL_DeleteContext(m_gContext);
            m_gContext = NULL;
            delete[] m_activeTextures;
            m_activeTextures = NULL;
        }

        void Graphics::SetViewport(int width, int height) {
            GLTrackCall(glViewport(0, 0, width, height));
            m_cachedScreenResolution = glm::ivec2(width, height);
        }

        void Graphics::SetShader(Shader* shader) {
            if (m_currentCamera == nullptr) {
                Logger::Error("Graphics.SetShader - Trying to set shader without a active camera");
                return;
            }
            m_currentShader = shader;
            m_currentShader->Use();

            // Checking if shader uses view and perspective matrices before setting them.
            std::vector<ShaderUniform> shaderUniforms = m_currentShader->GetShaderUniforms();
            for (auto it = shaderUniforms.begin(); it != shaderUniforms.end(); ++it) {
                ShaderUniform uniform = *it;
                if (uniform.name == SHADERS_VIEW_MATRIX) {
                    glm::mat4 look = Camera::CameraLookMatrix(*m_currentCamera);
                    Graphics::SetShaderMatrix(SHADERS_VIEW_MATRIX, glm::value_ptr(look));
                } else if (uniform.name == SHADERS_PERSPECTIVE_MATRIX) {
                    glm::mat4 persp = Camera::CameraPerspectiveMatrix(*m_currentCamera);
                    Graphics::SetShaderMatrix(SHADERS_PERSPECTIVE_MATRIX, glm::value_ptr(persp));
                } else if (uniform.name == SHADERS_SCREEN_RESOLUTION) {
                    Graphics::SetShaderIVector2(SHADERS_SCREEN_RESOLUTION, glm::value_ptr(m_cachedScreenResolution));
                }
            }
        }

        void Graphics::SetShaderMatrix(const char* matrixName, const GLfloat* values) {
            if (!m_currentShader) {
                Logger::Error("Graphics.SetShaderMatrix - Trying to bind matrix with no shader attached.");
                return;
            }
            m_currentShader->Set4x4MatrixUniform(matrixName, values);
        }

        void Graphics::SetShaderIVector2(const char* vectorName, const GLint* values) {
            if (!m_currentShader) {
                Logger::Error("Graphics.SetShaderIVector2 - Trying to bind IVector2 with no shader attached.");
                return;
            }
            m_currentShader->SetIVec2Uniform(vectorName, values);
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
            GLTrackCall(glClearColor(201 / 255.0f, 226 / 255.0f, 255 / 255.0f, 1.0f));
            GLTrackCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        }

        void Graphics::SetCurrentCamera(Camera::Camera* camera) {
            m_currentCamera = camera;
        }

        SDL_Window* Graphics::m_window = nullptr;
        SDL_GLContext Graphics::m_gContext = nullptr;
        Shader* Graphics::m_currentShader = nullptr;
        Mesh* Graphics::m_currentMesh = nullptr;
        Texture** Graphics::m_activeTextures = nullptr;
        GLint Graphics::m_maxTexturesSlots = 0;
        Camera::Camera* Graphics::m_currentCamera = nullptr;
        glm::ivec2 Graphics::m_cachedScreenResolution = glm::ivec2(1024, 768);
        std::vector<std::string> Graphics::graphicsUniformsNames = std::vector<std::string>();

    }
}