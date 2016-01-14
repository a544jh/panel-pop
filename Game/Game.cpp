/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"

#include "Board.h"

Game::Game() :
		_paused(false), _ticksRun(0) {
}

void Game::tick() {
	if (!_paused) {
		++_ticksRun;
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
		b2.queueGarbage(true, b1.getLastChain() - 1, GarbageBlockType::NORMAL);
	}

	if (combo >= 4 && combo <= 7) {
		b2.queueGarbage(false, combo - 1, GarbageBlockType::NORMAL);
	} else if (combo == 8) {
		b2.queueGarbage(false, 3, GarbageBlockType::NORMAL);
		b2.queueGarbage(false, 4, GarbageBlockType::NORMAL);
	} else if (combo >= 9 && combo <= 10) {
		b2.queueGarbage(false, combo - 5, GarbageBlockType::NORMAL);
		b2.queueGarbage(false, combo - 5, GarbageBlockType::NORMAL);
	} else if (combo == 11) {
		b2.queueGarbage(false, 5, GarbageBlockType::NORMAL);
		b2.queueGarbage(false, 6, GarbageBlockType::NORMAL);
	} else if (combo >= 12) {
		for (int i = 0; i < 5 - combo; ++i)
			b2.queueGarbage(true, 1, GarbageBlockType::NORMAL);
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

