//
// Created by axel on 4/19/19.
//

#include <SDL_keyboard.h>
#include <sstream>
#include "KeyboardKey.h"
#include "../Debug/DebugInput.h"

KeyboardKey::KeyboardKey(int scancode) : _scancode(scancode) {}

bool KeyboardKey::isActive() const {
    // DebugInput lets the headless debug console inject keys; it is inert
    // unless --debug-stdin is active (see AGENTS.md).
    return DebugInput::active(_scancode)
        || SDL_GetKeyboardState(NULL)[_scancode];
}

std::string KeyboardKey::toString() {
    std::ostringstream os;
    os << "K" << SDL_GetScancodeName((SDL_Scancode) _scancode);
    return os.str();
}
