//
// Created by axel on 9/8/18.
//

#include <SDL_gamecontroller.h>
#include <SDL_scancode.h>
#include "InputState.h"
#include "InputEvents/KeyboardKey.h"
#include "Config/InputConfig.h"

InputState::InputState() : _direction(NONE),
                           _swap(false),
                           _raiseStack(false),
                           _start(false) {}

InputState::InputState(Direction direction, bool swap, bool raise_stack, bool start)
    : _direction(direction), _swap(swap), _raiseStack(raise_stack), _start(start) {}

InputState InputState::getCurrentState(InputConfig &inputConfig) {

    Direction direction = getDirection(inputConfig);
    bool swap = inputConfig._swap->isActive();
    bool raise = inputConfig._raiseStack->isActive();
    bool start = inputConfig._start->isActive();
    InputState state(direction, swap, raise, start);

    return state;
}
Direction InputState::getDirection(InputConfig &config) {
    if (config._up->isActive()) {
        return Direction::UP;
    } else if (config._down->isActive()) {
        return Direction::DOWN;
    } else if (config._left->isActive()) {
        return Direction::LEFT;
    } else if (config._right->isActive()) {
        return Direction::RIGHT;
    } else {
        return Direction::NONE;
    }
}
