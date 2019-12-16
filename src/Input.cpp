#include<cstring>

#include"Input.h"
#include"Utils/Logger.h"

Input::~Input() {
    if (m_PreviousKeysState) {
        delete[] m_PreviousKeysState;
        m_PreviousKeysState = nullptr;
    }
}

bool Input::Init(const Uint8* SDL_KeysPointer, int SDL_KeysCount) {
    m_SDLKeysPointer = SDL_KeysPointer;
    m_SDLKeysCount = SDL_KeysCount;

    if (m_SDLKeysPointer) {
        m_PreviousKeysState = new Uint8[m_SDLKeysCount];

        for (int i = 0; i < m_SDLKeysCount; i++) {
            m_PreviousKeysState[i] = m_SDLKeysPointer[i];
        }
    } else {
        Logger::Error("Input.Init: Keys arrays not initialized.");
        return false;
    }
    return true;
}

bool Input::GetKeyPressed(const char* keyName) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(keyName);
    if (scancode < 0 || scancode >= m_SDLKeysCount) {
        Logger::Error("Input.GetKeyPressed: Key's scancode out of keys array's bounds.");
        return false;
    }
    if (!m_SDLKeysPointer || !m_PreviousKeysState) {
        Logger::Error("Input.GetKeyPressed: Keys arrays not initialized.");
        return false;
    }
    return m_SDLKeysPointer[scancode];
}

bool Input::GetKeyUp(const char* keyName) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(keyName);
    if (scancode < 0 || scancode >= m_SDLKeysCount) {
        Logger::Error("Input.GetKeyUp: Key's scancode out of keys array's bounds.");
        return false;
    }
    if (!m_SDLKeysPointer || !m_PreviousKeysState) {
        Logger::Error("Input.GetKeyUp: Keys arrays not initialized.");
        return false;
    }
    if (!m_SDLKeysPointer[scancode] && m_PreviousKeysState[scancode]) {
        return true;
    }
    return false;
}

bool Input::GetKeyDown(const char* keyName) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(keyName);
    if (scancode < 0 || scancode >= m_SDLKeysCount) {
        Logger::Error("Input.GetKeyDown: Key's scancode out of keys array's bounds.");
        return false;
    }
    if (!m_SDLKeysPointer || !m_PreviousKeysState) {
        Logger::Error("Input.GetKeyDown: Keys arrays not initialized.");
        return false;
    }
    if (m_SDLKeysPointer[scancode] && !m_PreviousKeysState[scancode]) {
        return true;
    }
    return false;
}

void Input::GetMouseDelta(int *out_x, int *out_y) {
    if (m_cursorLocked) {
        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        *out_x = m_mouseX - dm.w / 2;
        *out_y = m_mouseY - dm.h / 2;
    } else {
        *out_x = m_mouseX - m_prevMouseX;
        *out_y = m_mouseY - m_prevMouseY;
    }
}

void Input::Update() {
    if (m_SDLKeysPointer) {
        std::memcpy(m_PreviousKeysState, m_SDLKeysPointer, m_SDLKeysCount * sizeof(Uint8));
    }
    m_prevMouseX = m_mouseX;
    m_prevMouseY = m_mouseY;
    SDL_GetGlobalMouseState(&m_mouseX, &m_mouseY);

    if (m_cursorLocked) {
        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        SDL_WarpMouseGlobal(dm.w / 2, dm.h / 2);
    }
}

void Input::MouseButtonDownEvent(const SDL_Event& event) {
    
}

void Input::MouseButtonUpEvent(const SDL_Event& event) {
    
}

bool Input::IsCursorLocked() {
    return m_cursorLocked;
}

void Input::LockCursor() {
    m_cursorLocked = true;
}

void Input::UnlockCursor() {
    m_cursorLocked = false;
}

int Input::m_SDLKeysCount = 0;
const Uint8* Input::m_SDLKeysPointer = nullptr;
Uint8* Input::m_PreviousKeysState = nullptr;
int Input::m_mouseX = 0;
int Input::m_mouseY = 0;
int Input::m_prevMouseX = 0;
int Input::m_prevMouseY = 0;
bool Input::m_cursorLocked = false;