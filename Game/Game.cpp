/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"
#include "../InputManager.h"
#include <SDL2/SDL.h>
#include <array>


Game::Game() {
}

void Game::tick() {
	InputManager& input = InputManager::getInstance();


	if (!_prevInput[SDL_SCANCODE_UP] && input._keys[SDL_SCANCODE_UP]) {
		_board.inputMoveCursor(UP);
	} else if (!_prevInput[SDL_SCANCODE_DOWN] && input._keys[SDL_SCANCODE_DOWN]) {
		_board.inputMoveCursor(DOWN);
	} else if (!_prevInput[SDL_SCANCODE_LEFT] && input._keys[SDL_SCANCODE_LEFT]) {
		_board.inputMoveCursor(LEFT);
	} else if (!_prevInput[SDL_SCANCODE_RIGHT] && input._keys[SDL_SCANCODE_RIGHT]) {
		_board.inputMoveCursor(RIGHT);
	}

	if(!_prevInput[SDL_SCANCODE_X] && input._keys[SDL_SCANCODE_X]) {
		_board.inputSwapBlocks();
	}
	if(input._keys[SDL_SCANCODE_S]) {
			_board.inputForceStackRaise();
		}

	_board.tick();

	_prevInput = input._keys;

}

Game::~Game() {
	// TODO Auto-generated destructor stub
}

