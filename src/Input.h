#pragma once
#include<SDL2/SDL.h>
#include<string>

class Input {
private:
    static int m_SDLKeysCount;
    static const Uint8* m_SDLKeysPointer;

    // Keeps a copy of the keys array to compare between frames.
    static Uint8* m_PreviousKeysState;

private:
    Input() = delete;
    ~Input();

public:
    static bool Init(const Uint8* SDL_KeysPointer, int SDL_KeysCount);
    static bool GetKeyPressed(const char* keyName);
    static bool GetKeyUp(const char* keyName);
    static bool GetKeyDown(const char* keyName);

    static void Update();
};