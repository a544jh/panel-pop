/*
 * KeyboardController.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "KeyboardController.h"

#include <SDL2/SDL.h>

#include "Board.h"

KeyboardController::KeyboardController(Board& b, const KeyConfig& c) :
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
    InputManager& input = InputManager::getInstance();
    Direction directions[] = {UP, DOWN, LEFT, RIGHT};

    if (!input.keyPressed(getDirectionKey(_directionHeld))) {
        _directionHeld = NONE;
    } else if (SDL_GetTicks() - _holdBegin >= REPEAT_MS) {
        _board.inputMoveCursor(_directionHeld);
    }

    if (_directionHeld == NONE || SDL_GetTicks() - _holdBegin < REPEAT_MS) {
        for (int i = 0; i < 4; ++i) {
            if (input.keyDown(getDirectionKey(directions[i]))) {
                _board.inputMoveCursor(directions[i]);
                _directionHeld = directions[i];
                _holdBegin = SDL_GetTicks();
            }
        }
    }

    if (input.keyDown(_config.swap)) {
        _board.inputSwapBlocks();
    }
    if (input.keyPressed(_config.raiseStack)) {
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

