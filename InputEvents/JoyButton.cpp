//
// Created by axel on 4/21/19.
//

#include <sstream>
#include <SDL_joystick.h>
#include "JoyButton.h"
#include "../InputManager.h"
JoyButton::JoyButton(SDL_JoystickID joystick_id, int button_id)
    : _joystickId(joystick_id), _buttonId(button_id), _joystick(nullptr) {
    _joystick = InputManager::getInstance().getJoystick(joystick_id);
}

bool JoyButton::isActive() const {
    return SDL_JoystickGetButton(_joystick, _buttonId);
}

std::string JoyButton::toString() {
    std::ostringstream os;
    os << "J" << _joystickId << "_B" << _buttonId;
    return os.str();
}
