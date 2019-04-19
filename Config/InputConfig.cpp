//
// Created by axel on 9/8/18.
//

#include "InputConfig.h"

InputConfig::InputConfig(const InputEvent &up,
                         const InputEvent &down,
                         const InputEvent &left,
                         const InputEvent &right,
                         const InputEvent &swap,
                         const InputEvent &raiseStack) :
    _up(up),
    _down(down),
    _left(left),
    _right(right),
    _swap(swap),
    _raiseStack(raiseStack) {}
