/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"

#include <SDL2/SDL_timer.h>

#include "GarbageBlock.h"

Game::Game() :
				_board(this),
				_board2(this),
				_paused(false),
				_ticksRun(0),
				_advanceTick(false),
				_startTime(SDL_GetTicks()),
				_pausedTime(0),
				_p1MatchPoints(0),
				_p2MatchPoints(0),
				_p1Points(0),
				_p2Points(0) {
}

void Game::tick() {
	if (!_paused || _advanceTick) {
		_advanceTick = false;
		++_ticksRun;
		handleGarbageSpawning(_board, _board2);
		handleGarbageSpawning(_board2, _board);
		_board.tick();
		_board2.tick();

		if (_board.getState() == Board::GAME_OVER
				|| _board2.getState() == Board::GAME_OVER) {
			if (_board.getState() == Board::RUNNING) {
				_board.win();
				++_p1MatchPoints;
			} else if (_board2.getState() == Board::RUNNING) {
				_board2.win();
				++_p2MatchPoints;
			}
			if (_p1MatchPoints >= MATCH_POINTS) {
				_p1MatchPoints = 0;
				_p2MatchPoints = 0;
				++_p1Points;
			}
			if (_p2MatchPoints >= MATCH_POINTS) {
				_p1MatchPoints = 0;
				_p2MatchPoints = 0;
				++_p2Points;
			}
			inputTogglePause();
		}
	}
}

bool Game::isAdvanceTick() const {
	return _advanceTick;
}

int Game::getP1MatchPoints() const {
	return _p1MatchPoints;
}

int Game::getP1Points() const {
	return _p1Points;
}

int Game::getP2MatchPoints() const {
	return _p2MatchPoints;
}

int Game::getP2Points() const {
	return _p2Points;
}

void Game::handleGarbageSpawning(Board& b1, Board& b2) {
//TODO: make it like the actual game

	int combo = b1.getTickMatched();

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
	_paused = false;
	_startTime = SDL_GetTicks();
	_board = Board(this); //fix this :P
	_board2 = Board(this);
}

void Game::inputTogglePause() {
	if (!_paused) {
		_pausedTime = SDL_GetTicks() - _startTime;
		_startTime = 0;
	} else {
		_startTime = SDL_GetTicks() - _pausedTime;
		_pausedTime = 0;
	}
	_paused = !_paused;
}

uint32_t Game::getTime() {
	if (_paused) {
		return _pausedTime;
	}
	return SDL_GetTicks() - _startTime;
}

void Game::inputAdvanceTick() {
	_advanceTick = true;
}

const bool Game::isPaused() const {
	return _paused;
}

Game::~Game() {
}

