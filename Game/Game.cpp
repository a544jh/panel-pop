/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"

#include "Board.h"

Game::Game() :
		_paused(false) {
}

void Game::tick() {
	if (!_paused) {
		handleGarbageSpawning(_board, _board2);
		handleGarbageSpawning(_board2, _board);
		_board.tick();
		_board2.tick();
	}
}

void Game::handleGarbageSpawning(Board& b1, Board& b2) {
	//TODO: make it like the actual game

	int combo = b1.getTickMatched();
	int y = Board::BOARD_HEIGHT - 1;

	if (b1.isTickChainEnd()) {
		b2.spawnGarbage(0, y, Board::BOARD_WIDTH, b1.getLastChain() - 1,
				GarbageBlockType::NORMAL);
	}

	if (combo >= 4 && combo <= 5) {
		b2.spawnGarbage(0, y, combo, 1, GarbageBlockType::NORMAL);
	}
	if (b1.getTickMatched() >= 6) {
		for (int i = 0; i < 5 - combo; ++i)
			b2.spawnGarbage(0, y - i, Board::BOARD_WIDTH, 1, GarbageBlockType::NORMAL);
	}
}

void Game::reset() {
	_board = Board(); //fix this :P
	_board2 = Board();
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

