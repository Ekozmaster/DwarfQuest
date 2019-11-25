#pragma once
#include<SDL2/SDL.h>

class Window {
private:
    SDL_Window* m_gWindow = NULL;
    unsigned int m_width = 640;
    unsigned int m_height = 480;

public:
    bool Init();
    void Render();
    void Close();
    void SetSize(unsigned int width, unsigned int height);
};