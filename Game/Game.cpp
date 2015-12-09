/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"
#include "Board.h"


Game::Game() : _board(Board()), _kbc(KeyboardController(_board)) {
}

void Game::tick() {
	_kbc.tick();
}

Game::~Game() {
	// TODO Auto-generated destructor stub
}

