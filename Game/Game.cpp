/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"

#include "Board.h"

Game::Game() : _paused(false) {
}

void Game::tick() {
	if (!_paused) {
		_board.tick();
	}
}

void Game::reset(){
	_board = Board(); //fix this :P
}

void Game::inputTogglePause() {
	_paused = !_paused;
}

const bool Game::isPaused() const {
	return _paused;
}

Game::~Game() {
	// TODO Auto-generated destructor stub
}

