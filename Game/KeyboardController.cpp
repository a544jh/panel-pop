/*
 * KeyboardController.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "KeyboardController.h"
#include "../Config/InputConfig.h"

#include <SDL2/SDL.h>

#include "Board.h"
#include "../InputEvents/KeyboardKey.h"

KeyboardController::KeyboardController(Board &b, InputConfig &c) :
        BoardController(b),
        _config(c),
        _directionHeld(NONE),
        _holdBegin(0) {
}

void KeyboardController::tick() {

    if (_config._currentState._direction != _directionHeld) {
        _directionHeld = _config._currentState._direction;
        _holdBegin = SDL_GetTicks();
        _board.inputMoveCursor(_directionHeld);
    } else if (SDL_GetTicks() - _holdBegin >= REPEAT_MS) {
        _board.inputMoveCursor(_directionHeld);
    }

    if ((_config.swapDown())) {
        _board.inputSwapBlocks();
    }
    if (_config.raiseStackDown()) {
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
}

KeyboardController::~KeyboardController() {
}

