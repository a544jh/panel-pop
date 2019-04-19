//
// Created by axel on 9/8/18.
//

#include <SDL_gamecontroller.h>
#include <SDL_scancode.h>
#include "InputState.h"
#include "InputEvents/KeyboardKey.h"

InputState::InputState(Direction direction, bool swap, bool raiseStack) : _direction(direction),
                                                                          _swap(swap),
                                                                          _raiseStack(raiseStack) {}

InputState InputState::getCurrentState(InputConfig &inputConfig) {

  Direction direction = getDirection(inputConfig);

  bool swap = inputConfig._swap.isActive();

  bool raise = inputConfig._raiseStack.isActive();

  InputState state = InputState(direction, swap, raise);

  return state;
}

Direction InputState::getDirection(InputConfig &config) {
  if (config._up.isActive()) {
    return Direction::UP;
  } else if (config._down.isActive()) {
    return Direction::DOWN;
  } else if (config._left.isActive()) {
    return Direction::LEFT;
  } else if (config._right.isActive()) {
    return Direction::RIGHT;
  } else {
    return Direction::NONE;
  }
}
