#pragma once

#include "Window.h"
#include "Graphics/OpenGL/Mesh.h"
#include "Graphics/OpenGL/Shader.h"

class Application {
private:
    Window* window;
    SDL_Event event;
    bool running = false;

    Mesh mesh;
    Shader shader;

    void FrameSetupStage();
    void FrameMainStage();
    void FrameFinishStage();
    void FrameRenderStage();
public:
    bool Init();
    int Run();
    void Destroy();
    void DispatchEvents();
};