#include <iostream>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include "Application.h"
#include "Utils/Logger.h"
#include "Input.h"
#include "Graphics/OpenGL/Camera.h"
#include "Graphics/OpenGL/Graphics.h"
#include "Graphics/OpenGL/ShadersDefinitions.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/Mesh.h"
#include "ResourceManagement/MeshLoader.h"
#include "ResourceManagement/ImageLoader.h"

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

    if (Input::GetKeyPressed("W")) camera.position += camera.direction * 0.01666f;
    if (Input::GetKeyPressed("S")) camera.position += -camera.direction * 0.01666f;
    if (Input::GetKeyPressed("A")) camera.position += Camera::Right(camera) * -0.01666f;
    if (Input::GetKeyPressed("D")) camera.position += Camera::Right(camera) * 0.01666f;

    if (Input::GetKeyDown("P")) running = false;
    if (Input::GetKeyDown("E")) Input::IsCursorLocked() ? Input::UnlockCursor() : Input::LockCursor();
}

void Application::FrameFinishStage() {
    Input::Update();
}

void Application::FrameRenderStage() {
    Graphics::ClearScreen();

    // Setting up shader.
    Graphics::SetShader(&shader);
    glm::mat4 persp = Camera::CameraPerspectiveMatrix(camera);
    glm::mat4 look = Camera::CameraLookMatrix(camera);
    glm::mat4 identity(1);
    Graphics::SetShaderMatrix(SHADERS_MODEL_MATRIX, glm::value_ptr(identity));
    Graphics::SetShaderMatrix(SHADERS_VIEW_MATRIX, glm::value_ptr(look));
    Graphics::SetShaderMatrix(SHADERS_PERSPECTIVE_MATRIX, glm::value_ptr(persp));

    // Rendering.
    Graphics::SetTexture(texture, 0);
    Graphics::SetMesh(mesh);
    Graphics::RenderMesh();
    window->SwapBuffers();
}

bool Application::Init() {
    Logger::SetLoggingLevel(Logger::LoggingLevelInfo);
    window = new Window();
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
    const GLfloat vertices[] = {
        -0.5f, -0.5f,  0.0f, // VERTEX 1
         1.0f,  0.0f,  0.0f, // NORMAL 1
         0.0f,  0.0f,        // UV 1
         0.0f,  0.5f,  0.0f, // VERTEX 2
         0.0f,  1.0f,  0.0f, // NORMAL 2
         0.5f,  1.0f,        // UV 2
         0.5f, -0.5f,  0.0f, // VERTEX 3
         0.0f,  0.0f,  1.0f, // NORMAL 3
         1.0f,  0.0f,        // UV 3
    };
    
    const GLuint indices[] = {
        0, 1, 2
    };
    
    Logger::Info("Creating Mesh");
    //mesh.Create(vertices, 9, indices, 3);
    mesh = LoadMesh("Assets/Models/TestCrateModel.obj");
    texture = LoadTexture("Assets/Textures/TestCrateAOBake.jpg");
    //mesh = LoadMesh("Assets/Models/test2.obj");
    
    shader.CompileShaders("Assets/Shaders/testVertexShader.glsl", "Assets/Shaders/testFragmentShader.glsl");
    // </TESTING>






    return initStatus;
}

int Application::Run() {
    running = true;
    float mouseX = 0;
    float mouseY = 0;
    
    camera = Camera::InitADefaultCamera();

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
    delete mesh;
    delete texture;
    shader.Destroy();

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