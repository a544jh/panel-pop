//
// Created by axel on 9/8/18.
//

#include "InputConfig.h"

InputConfig::InputConfig(InputEvent *up,
                         InputEvent *down,
                         InputEvent *left,
                         InputEvent *right,
                         InputEvent *swap,
                         InputEvent *raiseStack,
                         InputEvent *start) :
    _up(up),
    _down(down),
    _left(left),
    _right(right),
    _swap(swap),
    _raiseStack(raiseStack),
    _start(start) {}

void InputConfig::updateState() {
    _prevState = _currentState;
    _currentState = InputState::getCurrentState(*this);
}

Direction InputConfig::getDownDirection() const {
    if (_prevState._direction != _currentState._direction) {
        return _currentState._direction;
    }
    return NONE;
}

bool InputConfig::raiseStackDown() const {
    return !_prevState._raiseStack && _currentState._raiseStack;
}

bool InputConfig::swapDown() const {
    return !_prevState._swap && _currentState._swap;
}

bool InputConfig::startDown() const {
    return !_prevState._start && _currentState._start;
}



