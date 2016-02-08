/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "Game.h"

#include <SDL2/SDL_timer.h>

#include "BoardEventHandler.h"
#include "GameEventHandler.h"
#include "GarbageBlock.h"

Game::Game(GameEventHandler* geh, BoardEventHandler* beh1,
		BoardEventHandler* beh2) :
				_eventHandler(geh),
				_beh1(beh1),
				_beh2(beh2),
				_board(this, beh1),
				_board2(this, beh2),
				_state(State::RUNNING),
				_ticksRun(0),
				_advanceTick(false),
				_startTime(SDL_GetTicks()),
				_pausedTime(0),
				_p1MatchPoints(0),
				_p2MatchPoints(0),
				_p1Points(0),
				_p2Points(0),
				_pauseMenu(*this) {
	_eventHandler->gameReset();
}

void Game::handleEnd() {
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
		_pausedTime = SDL_GetTicks() - _startTime;
		_state = State::ENDED;
	}
}

void Game::tick() {

	if (_board.getState() == Board::COUNTDOWN) {
		_eventHandler->countdown(getTime());
	}

	if (_state == State::RUNNING || _advanceTick) {
		_advanceTick = false;
		++_ticksRun;
		handleGarbageSpawning(_board, _board2);
		handleGarbageSpawning(_board2, _board);
		_board.tick();
		_board2.tick();

		//handleEnd();
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

PauseMenu& Game::getPauseMenu() {
	return _pauseMenu;
}

void Game::handleGarbageSpawning(Board& b1, Board& b2) {

	int combo = b1.getTickMatched();
	int chain = b1.getLastChain() - 1;
	if (chain > 12) {
		chain = 12;
	}

	if (b1.isTickChainEnd()) {
		b2.queueGarbage(true, chain, GarbageBlockType::NORMAL);
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
	_eventHandler->gameReset();
	_state = State::RUNNING;
	_startTime = SDL_GetTicks();
	_board = Board(this, _beh1); //fix this :P
	_board2 = Board(this, _beh2);
}

void Game::inputTogglePause() {
	if (_state == State::RUNNING) {
		_pausedTime = SDL_GetTicks() - _startTime;
		_startTime = 0;
		_pauseMenu.resetCursor();
		_eventHandler->gamePause();
		_state = State::PAUSED;
	} else if (_state == State::PAUSED) {
		_startTime = SDL_GetTicks() - _pausedTime;
		_pausedTime = 0;
		_eventHandler->gameResume();
		_state = State::RUNNING;
	}
}

uint32_t Game::getTime() {
	if (_state != State::RUNNING) {
		return _pausedTime;
	}
	return SDL_GetTicks() - _startTime;
}

void Game::inputAdvanceTick() {
	_advanceTick = true;
}

const bool Game::isPaused() const {
	return _state == State::PAUSED;
}

Game::~Game() {
	delete _eventHandler;
	delete _beh1;
	delete _beh2;
}

Game::State Game::getState() const {
	return _state;
}
