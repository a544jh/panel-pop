/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"

#include <SDL_scancode.h>
#include "../InputManager.h"
#include "Board.h"


Game::Game() : _board(Board()), _kbc(KeyboardController(_board)), _paused(false) {
}

void Game::tick() {
	InputManager& input = InputManager::getInstance();
	if(input._keys[SDL_SCANCODE_P] && !input._prevKeys[SDL_SCANCODE_P]){
		_paused = !_paused;
	}
	if(!_paused){
		_kbc.tick();
	}
}

bool Game::isPaused(){
	return _paused;
}

Game::~Game() {
	// TODO Auto-generated destructor stub
}

