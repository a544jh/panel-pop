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

KeyboardController::KeyboardController(Board &b, const KeyConfig &c) :
        BoardController(b),
        _config(c),
        _directionHeld(NONE),
        _holdBegin(0) {
}

int KeyboardController::getDirectionKey(Direction d) {
    switch (d) {
        case UP:
            return _config.up;
            break;
        case DOWN:
            return _config.down;
            break;
        case LEFT:
            return _config.left;
            break;
        case RIGHT:
            return _config.right;
            break;
        default:
            return 0;
            break;
    }
}

void KeyboardController::tick() {


    InputState state = InputState::getCurrentState();

    InputManager &input = InputManager::getInstance();

    if (state._direction != _directionHeld) {
        _directionHeld = state._direction;
        _holdBegin = SDL_GetTicks();
        _board.inputMoveCursor(_directionHeld);
    } else if (SDL_GetTicks() - _holdBegin >= REPEAT_MS) {
        _board.inputMoveCursor(_directionHeld);
    }

    if (input.keyDown(_config.swap) || state._swap) {
        _board.inputSwapBlocks();
    }
    if (input.keyPressed(_config.raiseStack) || state._raiseStack) {
        _board.inputForceStackRaise();
    }

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
    }
}

KeyboardController::~KeyboardController() {
}

