#pragma once
#include<SDL2/SDL.h>
#include<string>

class Input {
private:
    static int m_SDLKeysCount;
    static const Uint8* m_SDLKeysPointer;

    // Keeps a copy of the keys array to compare between frames.
    static Uint8* m_PreviousKeysState;
    
    static int m_mouseX, m_mouseY, m_prevMouseX, m_prevMouseY;
    static bool m_cursorLocked;

private:
    Input() = delete;
    ~Input();

public:
    static bool Init(const Uint8* SDL_KeysPointer, int SDL_KeysCount);
    static bool GetKeyPressed(const char* keyName);
    static bool GetKeyUp(const char* keyName);
    static bool GetKeyDown(const char* keyName);
    static void GetMouseDelta(int *out_x, int *out_y);

    static void Update();
    static void MouseButtonDownEvent(const SDL_Event& event);
    static void MouseButtonUpEvent(const SDL_Event& event);
    static bool IsCursorLocked();
    static void LockCursor();
    static void UnlockCursor();
};