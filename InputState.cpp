//
// Created by axel on 9/8/18.
//

#include <SDL_gamecontroller.h>
#include "InputState.h"

InputState::InputState(Direction direction, bool swap, bool raiseStack) : _direction(direction),
                                                                                      _swap(swap),
                                                                                      _raiseStack(raiseStack) {}

Direction getControllerDirection() {

    SDL_GameController *ctrl = SDL_GameControllerOpen(0);

    if (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        return Direction::UP;
    } else if (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
        return Direction::DOWN;
    } else if (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
        return Direction::LEFT;
    } else if (SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        return Direction::RIGHT;
    }

    return Direction::NONE;

}

bool getControllerSwap() {
    SDL_GameController *ctrl = SDL_GameControllerOpen(0);

    return SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_A);
}

bool getControllerStackRaise() {
    SDL_GameController *ctrl = SDL_GameControllerOpen(0);

    return SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_B) || SDL_GameControllerGetButton(ctrl, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}

InputState InputState::getCurrentState() {

    Direction direction = getControllerDirection();

    bool swap = getControllerSwap();

    bool raise = getControllerStackRaise();

    InputState state = InputState(direction, swap, raise);

    return state;
}