#include <iostream>

#include "Application.h"
#include "Utils/Logger.h"
#include "Input.h"

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
    return initStatus;
}

#include"Graphics/OpenGL/Camera.h"
#include "Graphics/OpenGL/Graphics.h"
#include<glm/gtc/type_ptr.hpp>
#include<GL/gl.h>
#include<chrono>

int Application::Run() {
    running = true;
    float mouseX = 0;
    float mouseY = 0;
    
    Camera::Camera camera = Camera::InitADefaultCamera();

    while (running) {
        auto frameStartTime = std::chrono::high_resolution_clock::now();
        // #### Setup stage.
        DispatchEvents();


        
        // #### Main stage.
        int mouseXDelta, mouseYDelta;
        Input::GetMouseDelta(&mouseXDelta, &mouseYDelta);
        mouseX += mouseXDelta;
        mouseY += mouseYDelta;
        
        glm::quat camRotation = glm::quat(glm::vec3(mouseY * 0.0005, -mouseX * 0.0005, 0));
        camera.direction = camRotation * glm::vec3(0.0f, 0.0f, 1.0f);
        
        if(Input::GetKeyPressed("W")) camera.position += camera.direction * 0.01666f;
        else if(Input::GetKeyPressed("S")) camera.position += -camera.direction * 0.01666f;
        else if(Input::GetKeyPressed("A")) camera.position += Camera::Right(camera) * -0.01666f;
        else if(Input::GetKeyPressed("D")) camera.position += Camera::Right(camera) * 0.01666f;

        if (Input::GetKeyDown("P")) running = false;
        if (Input::GetKeyDown("E")) Input::IsCursorLocked() ? Input::UnlockCursor() : Input::LockCursor();

        
        // #### Finish stage.
        Input::Update();
        

        // #### Render stage.
        glm::mat4 persp = Camera::CameraPerspectiveMatrix(camera);
        glm::mat4 look = Camera::CameraLookMatrix(camera);
        Graphics::SetCameraView(glm::value_ptr(look), glm::value_ptr(persp));
        window->Render();
        
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = frameEndTime-frameStartTime;
        double deltaTime = diff.count();
        // std::cout << deltaTime << std::endl;
        // if(deltaTime < 1.0/60.0) SDL_Delay((1.0/60.0 - deltaTime) * 1000);
    }
    window->Close();
    return 0;
}

void Application::Destroy() {
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