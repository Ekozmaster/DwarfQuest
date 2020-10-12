#include <iostream>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <src/Application.h>
#include <src/Utils/Logger.h>
#include <src/Input.h>
#include <src/Graphics/OpenGL/Camera.h>
#include <src/Graphics/OpenGL/Graphics.h>
#include <src/Graphics/OpenGL/ShadersDefinitions.h>
#include <src/ResourceManagement/ResourceManager.h>
#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/Renderer.h>

namespace DwarfQuest {
    namespace Core {

        void Application::FrameSetupStage() {
            DispatchEvents();
        }

        int mouseX, mouseY;
        Camera::Camera camera;

        void Application::FrameMainStage() {

            if (Input::IsCursorLocked()) {
                int mouseXDelta, mouseYDelta;
                Input::GetMouseDelta(&mouseXDelta, &mouseYDelta);
                mouseX += mouseXDelta;
                mouseY += mouseYDelta;

                glm::quat camRotation = glm::quat(glm::vec3(mouseY * 0.0005, -mouseX * 0.0005, 0));
                camera.direction = camRotation * glm::vec3(0.0f, 0.0f, 1.0f);
            }

            float cameraSpeed = 2.5f;
            if (Input::GetKeyPressed("W")) camera.position += camera.direction * cameraSpeed;
            if (Input::GetKeyPressed("S")) camera.position += -camera.direction * cameraSpeed;
            if (Input::GetKeyPressed("A")) camera.position += Camera::Right(camera) * -cameraSpeed;
            if (Input::GetKeyPressed("D")) camera.position += Camera::Right(camera) * cameraSpeed;

            if (Input::GetKeyDown("P")) running = false;
            if (Input::GetKeyDown("E")) Input::IsCursorLocked() ? Input::UnlockCursor() : Input::LockCursor();

            terrain->SetPivotChunk(glm::ivec2(camera.position.x / CHUNK_WIDTH, camera.position.z / CHUNK_WIDTH));

            scene->Update();
        }

        void Application::FrameFinishStage() {
            Input::Update();
        }

        void Application::FrameRenderStage() {
            Graphics::ClearScreen();
            Graphics::SetCurrentCamera(&camera);
            scene->Render();
            window->SwapBuffers();
        }

        bool Application::Init() {
            Logger::SetLoggingLevel(Logger::LoggingLevelInfo);
            window = new Window(1024, 768);
            bool initStatus = window->Init();
            if (initStatus) {
                // Initializing Input module with SDL's internal keys array.
                int numberOfKeys;
                const Uint8* keysArrayPointer = SDL_GetKeyboardState(&numberOfKeys);
                initStatus = Input::Init(keysArrayPointer, numberOfKeys);

                if (!initStatus) {
                    window->Close();
                    return initStatus;
                }
            }





            // <TESTING>
            Logger::Info("Loading mock scene");
            scene = new Scene();
            scene->Init();

            // Test Scene
            auto gmIt = scene->NewGameObject("Voxel Terrain");
            terrain = (*gmIt).AddComponent<GameComponents::VoxelTerrain>();

            Logger::Info("Mock scene loaded");

            camera = Camera::InitADefaultCamera(window);
            camera.position = glm::vec3(0, 55, 0);

            
            // </TESTING>






            return initStatus;
        }

        int Application::Run() {
            running = true;
            float mouseX = 0;
            float mouseY = 0;

            while (running) {
                auto frameStartTime = std::chrono::high_resolution_clock::now();

                FrameSetupStage();
                FrameMainStage();
                FrameFinishStage();
                FrameRenderStage();

                auto frameEndTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = frameEndTime - frameStartTime;
                double deltaTime = diff.count();
                // std::cout << deltaTime << std::endl;
                // if(deltaTime < 1.0/60.0) SDL_Delay((1.0/60.0 - deltaTime) * 1000);
            }
            return 0;
        }

        void Application::Destroy() {
            ResourceManager::DestroyAssets();
            delete scene;
            if (window) {
                window->Close();
                delete window;
                window = nullptr;
            }
        }

        void Application::DispatchEvents() {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    Input::MouseButtonDownEvent(event);
                    break;
                case SDL_MOUSEBUTTONUP:
                    Input::MouseButtonUpEvent(event);
                    break;
                }
            }
        }

    }
}