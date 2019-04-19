//
// Created by axel on 9/8/18.
//

#include "InputConfig.h"

InputConfig::InputConfig(InputEvent *up,
                         InputEvent *down,
                         InputEvent *left,
                         InputEvent *right,
                         InputEvent *swap,
                         InputEvent *raiseStack) :
    _up(up),
    _down(down),
    _left(left),
    _right(right),
    _swap(swap),
    _raiseStack(raiseStack) {}
