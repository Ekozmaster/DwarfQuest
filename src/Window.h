#pragma once
#include<SDL2/SDL.h>

namespace DwarfQuest {
    namespace Core {

        class Window {
        private:
            SDL_Window* m_gWindow = NULL;
            unsigned int m_width = 640;
            unsigned int m_height = 480;

        public:
            bool Init();
            void SwapBuffers();
            void Close();
            void SetSize(unsigned int width, unsigned int height);
        };

    }
}