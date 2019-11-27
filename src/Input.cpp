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

void Input::GetMouseDelta(int& out_x, int& out_y) {
    out_x = m_mouseX - m_prevMouseX;
    out_y = m_mouseY - m_prevMouseY;
}

void Input::Update() {
    if (m_SDLKeysPointer) {
        std::memcpy(m_PreviousKeysState, m_SDLKeysPointer, m_SDLKeysCount * sizeof(Uint8));
    }
}

void Input::MouseMotionEvent(const SDL_Event& event) {
    Input::m_prevMouseX = Input::m_mouseX;
    Input::m_prevMouseY = Input::m_mouseY;
    Input::m_mouseX = event.motion.x;
    Input::m_mouseY = event.motion.y;
}

void Input::MouseButtonDownEvent(const SDL_Event& event) {
    
}

void Input::MouseButtonUpEvent(const SDL_Event& event) {
    
}

int Input::m_SDLKeysCount = 0;
const Uint8* Input::m_SDLKeysPointer = nullptr;
Uint8* Input::m_PreviousKeysState = nullptr;
int Input::m_mouseX = 0;
int Input::m_mouseY = 0;
int Input::m_prevMouseX = 0;
int Input::m_prevMouseY = 0;