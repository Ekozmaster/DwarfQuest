#pragma once

#include<SDL2/SDL.h>

class Graphics {
private:
    static SDL_Window* m_window;
    static SDL_GLContext m_gContext;

public:
    static bool Init(SDL_Window* attachedWindow);
    static void Render();
    static void Destroy();

    static void SetViewport(int width, int height);
};