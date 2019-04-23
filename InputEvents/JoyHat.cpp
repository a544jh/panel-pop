//
// Created by axel on 4/22/19.
//

#include <sstream>
#include "JoyHat.h"
#include "../InputManager.h"

JoyHat::JoyHat(SDL_JoystickID joystick_id, int hat_id, Uint8 hat_direction)
    : _joystickId(joystick_id),
      _hatId(hat_id),
      _joystick(nullptr),
      _hatDirection(hat_direction) {
    _joystick = InputManager::getInstance().getJoystick(joystick_id);
}
bool JoyHat::isActive() const {
    return SDL_JoystickGetHat(_joystick, _hatId) == _hatDirection;
}

std::string JoyHat::toString() {
    std::ostringstream os;
    os << "J" << _joystickId << "_H" <<_hatId << "_" << _hatDirection;
    return os.str();
}
