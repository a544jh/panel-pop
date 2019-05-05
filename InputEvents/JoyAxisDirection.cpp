//
// Created by axel on 4/22/19.
//

#include <sstream>
#include "JoyAxisDirection.h"
#include "../InputManager.h"
JoyAxisDirection::JoyAxisDirection(SDL_JoystickID joystick_id, int axis_id, JoyAxisDirection::Direction direction) :
    _joystickId(joystick_id), _axisId(axis_id), _direction(direction), _joystick(nullptr) {
    _joystick = InputManager::getInstance().getJoystick(joystick_id);
}

bool JoyAxisDirection::isActive() const {
    Sint16 value = SDL_JoystickGetAxis(_joystick,_axisId);
    if (_direction == POSITIVE) {
        return value > AXIS_THRESHOLD;
    } else {
        return value < -AXIS_THRESHOLD;
    }
}

std::string JoyAxisDirection::toString() {

    char direction = _direction == POSITIVE ? '+' : '-';

    std::ostringstream os;
    os << "J" << _joystickId << "_A" << _axisId << "_" << direction;
    return os.str();
}
