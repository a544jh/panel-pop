/*
 * KeyboardController.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "KeyboardController.h"
#include "../InputState.h"

#include <SDL2/SDL.h>

#include "Board.h"
#include "../InputEvents/KeyboardKey.h"

KeyboardController::KeyboardController(Board &b, const KeyConfig &c) :
    BoardController(b),
    _config(c),
    _directionHeld(NONE),
    _holdBegin(0),
    _prevState(Direction::DOWN, false, false) {
}



void KeyboardController::tick() {

    // TODO: use loaded config....
    InputConfig defaultConfig((KeyboardKey(SDL_SCANCODE_UP)),
                              KeyboardKey(SDL_SCANCODE_DOWN),
                              KeyboardKey(SDL_SCANCODE_LEFT),
                              KeyboardKey(SDL_SCANCODE_RIGHT),
                              KeyboardKey(SDL_SCANCODE_X),
                              KeyboardKey(SDL_SCANCODE_Z));

    InputState state = InputState::getCurrentState(defaultConfig);

    //InputManager &input = InputManager::getInstance();

    if (state._direction != _directionHeld) {
        _directionHeld = state._direction;
        _holdBegin = SDL_GetTicks();
        _board.inputMoveCursor(_directionHeld);
    } else if (SDL_GetTicks() - _holdBegin >= REPEAT_MS) {
        _board.inputMoveCursor(_directionHeld);
    }

    if ((state._swap && !_prevState._swap)) {
        _board.inputSwapBlocks();
    }
    if (state._raiseStack) {
        _board.inputForceStackRaise();
    }

    /*
    //debug stuff...
    if (input.keyDown(SDL_SCANCODE_1)) {
        _board.queueGarbage(false, 3, GarbageBlockType::NORMAL);
    }
    if (input.keyDown(SDL_SCANCODE_2)) {
        _board.queueGarbage(false, 4, GarbageBlockType::NORMAL);
    }
    if (input.keyDown(SDL_SCANCODE_3)) {
        _board.queueGarbage(false, 5, GarbageBlockType::NORMAL);
    }
    if (input.keyDown(SDL_SCANCODE_4)) {
        _board.queueGarbage(false, 6, GarbageBlockType::NORMAL);
    }
    if (input.keyDown(SDL_SCANCODE_6)) {
        _board.queueGarbage(true, 2, GarbageBlockType::NORMAL);
    }
    if (input.keyDown(SDL_SCANCODE_7)) {
        _board.queueGarbage(true, 3, GarbageBlockType::NORMAL);
    }
    if (input.keyDown(SDL_SCANCODE_8)) {
        _board.queueGarbage(true, 12, GarbageBlockType::NORMAL);
    }*/

    _prevState = state;
}

KeyboardController::~KeyboardController() {
}

