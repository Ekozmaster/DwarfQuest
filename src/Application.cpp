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
    
    Camera::Camera camera = Camera::InitADefaultCamera();

    while (running) {
        auto frameStartTime = std::chrono::high_resolution_clock::now();
        // #### Setup stage.
        DispatchEvents();


        // #### Main stage.
        
        if(Input::GetKeyPressed("W")) camera.position += glm::vec3(0.0, 0.0, 1.0) * 0.01666f;
        else if(Input::GetKeyPressed("S")) camera.position += glm::vec3(0.0, 0.0, -1.0) * 0.01666f;
        else if(Input::GetKeyPressed("A")) camera.position += Camera::Right(camera) * -0.01666f;
        else if(Input::GetKeyPressed("D")) camera.position += Camera::Right(camera) * 0.01666f;
        
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
        case SDL_MOUSEMOTION:
            Input::MouseMotionEvent(event);
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