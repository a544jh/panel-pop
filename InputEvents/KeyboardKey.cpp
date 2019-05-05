//
// Created by axel on 4/19/19.
//

#include <SDL_keyboard.h>
#include <sstream>
#include "KeyboardKey.h"

KeyboardKey::KeyboardKey(int scancode) : _scancode(scancode) {}

bool KeyboardKey::isActive() const {
    return SDL_GetKeyboardState(NULL)[_scancode];
}

std::string KeyboardKey::toString() {
    std::ostringstream os;
    os << "K" << SDL_GetScancodeName((SDL_Scancode) _scancode);
    return os.str();
}
