#pragma once
#include "Window.h"

class Application {
private:
    Window* window;
    SDL_Event event;
    bool running = false;
public:
    bool Init();
    int Run();
    void Destroy();
    void DispatchEvents();
};