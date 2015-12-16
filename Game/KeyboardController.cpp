/*
 * KeyboardController.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "KeyboardController.h"

#include <SDL2/SDL.h>

#include "Board.h"

KeyboardController::KeyboardController(Board& b) :
		BoardController(b) {
}

void KeyboardController::tick() {
	InputManager& input = InputManager::getInstance();

	if (!input._prevKeys[SDL_SCANCODE_UP] && input._keys[SDL_SCANCODE_UP]) {
		_board.inputMoveCursor(UP);
	} else if (!input._prevKeys[SDL_SCANCODE_DOWN]
			&& input._keys[SDL_SCANCODE_DOWN]) {
		_board.inputMoveCursor(DOWN);
	} else if (!input._prevKeys[SDL_SCANCODE_LEFT]
			&& input._keys[SDL_SCANCODE_LEFT]) {
		_board.inputMoveCursor(LEFT);
	} else if (!input._prevKeys[SDL_SCANCODE_RIGHT]
			&& input._keys[SDL_SCANCODE_RIGHT]) {
		_board.inputMoveCursor(RIGHT);
	}

	if (!input._prevKeys[SDL_SCANCODE_X] && input._keys[SDL_SCANCODE_X]) {
		_board.inputSwapBlocks();
	}
	if (input._keys[SDL_SCANCODE_S]) {
		_board.inputForceStackRaise();
	}
	if (!input._prevKeys[SDL_SCANCODE_1] && input._keys[SDL_SCANCODE_1]) {
		_board.spawnGarbage(0, _board.BOARD_HEIGHT - 1, 4, 1,
				GarbageBlockType::NORMAL);
	}
	if (!input._prevKeys[SDL_SCANCODE_2] && input._keys[SDL_SCANCODE_2]) {
		_board.spawnGarbage(0, _board.BOARD_HEIGHT - 1, _board.BOARD_WIDTH, 1,
				GarbageBlockType::NORMAL);
	}
	if (!input._prevKeys[SDL_SCANCODE_3] && input._keys[SDL_SCANCODE_3]) {
			_board.spawnGarbage(0, _board.BOARD_HEIGHT - 1, _board.BOARD_WIDTH, 3,
					GarbageBlockType::NORMAL);
		}
}

KeyboardController::~KeyboardController() {
	// TODO Auto-generated destructor stub
}

