#include "Application.h"
#include <iostream>
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

int Application::Run() {
    running = true;

    while (running) {
        // #### Setup stage.
        DispatchEvents();


        // #### Main stage.


        // #### Finish stage.
        Input::Update();
        

        // #### Render stage.
        window->Render();
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
        }
    }
}