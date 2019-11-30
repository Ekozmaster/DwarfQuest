#include <SDL2/SDL.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string>

#include "Window.h"
#include "Utils/Logger.h"
#include "Graphics/OpenGL/Graphics.h"

#ifdef DWARFQUEST_OPEN_GL
#include"Graphics/OpenGL/Graphics.h"
#endif

bool Window::Init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::Error("Window.Init: SDL could not initialize! SDL Error: " + std::string(SDL_GetError()));
        success = false;
    } else {
        int windowFlags = SDL_WINDOW_SHOWN;
#ifdef DWARFQUEST_OPEN_GL
        windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

        m_gWindow = SDL_CreateWindow("Dwarf Quest",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     m_width, m_height,
                                     windowFlags);
        if (m_gWindow == NULL) {
            Logger::Error("Window.Init: Window could not be created! SDL Error: " + std::string(SDL_GetError()));
            success = false;
        } else {
            success = Graphics::Init(m_gWindow);
            if (!success) {
                Logger::Error("Window.Init: Could not initialize Graphics module");
                SDL_DestroyWindow(m_gWindow);
            }
        }
    }
    SDL_ClearError();
    return success;
}

void Window::Render() {
    Graphics::Render();
    SDL_GL_SwapWindow(m_gWindow);
}

void Window::Close() {
    Graphics::Destroy();
    if (m_gWindow) SDL_DestroyWindow(m_gWindow);
    m_gWindow = NULL;
}

void Window::SetSize(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;
    SDL_SetWindowSize(m_gWindow, m_width, m_height);
    Graphics::SetViewport(m_width, m_height);
}