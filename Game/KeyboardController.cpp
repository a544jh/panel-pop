/*
 * KeyboardController.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "KeyboardController.h"

#include <SDL2/SDL.h>

#include "Board.h"

KeyboardController::KeyboardController(Board& b, KeyboardControllerConfig& c) :
		BoardController(b), _config(c) {
}

void KeyboardController::tick() {
	InputManager& input = InputManager::getInstance();

	if (input.keyDown(_config.up)) {
		_board.inputMoveCursor(UP);
	} else if (input.keyDown(_config.down)) {
		_board.inputMoveCursor(DOWN);
	} else if (input.keyDown(_config.left)) {
		_board.inputMoveCursor(LEFT);
	} else if (input.keyDown(_config.right)) {
		_board.inputMoveCursor(RIGHT);
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
		_board.queueGarbage(true, 3, GarbageBlockType::NORMAL);
	}
	if (input.keyDown(SDL_SCANCODE_7)) {
		_board.queueGarbage(true, 12, GarbageBlockType::NORMAL);
	}
}

KeyboardController::~KeyboardController() {
	// TODO Auto-generated destructor stub
}

