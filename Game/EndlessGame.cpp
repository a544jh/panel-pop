/*
 * EndlessGame.cpp
 *
 *  Created on: 24.2.2016
 *      Author: axelw
 */

#include "EndlessGame.h"

#include "GameEventHandler.h"

class BoardEventHandler;

EndlessGame::EndlessGame(GameEventHandler* geh, BoardEventHandler* beh) :
				Game(geh),
				_board(this, beh),
				_score(0),
				_highScore(0),
				_boardEventHandler(beh) {

}

EndlessGame::~EndlessGame() {
	delete _boardEventHandler;
}

void EndlessGame::reset() {
	_state = State::RUNNING;
	_startTime = SDL_GetTicks();
	_board = Board(this, _boardEventHandler);
	_panic = false;
	_eventHandler->gameReset();
}

void EndlessGame::tick() {
	if (_board.getState() == Board::COUNTDOWN) {
		_eventHandler->countdown(getTime());
	}

	if (_state == State::RUNNING) {
		++_ticksRun;
		_board.tick();
		if (!_panic && _board.isPanic()) {
			_panic = true;
			_eventHandler->panicBegin();
		}
		if (_panic && !_board.isPanic()) {
			_panic = false;
			_eventHandler->panicEnd();
		}

		handleEnd();
		_eventHandler->tickEnd();
	}
}

Board& EndlessGame::getBoard(int) {
	return _board;
}


int EndlessGame::getHighScore() const {
	return _highScore;
}

int EndlessGame::getScore() const {
	return _score;
}

void EndlessGame::handleEnd() {
	if (_board.getState() == Board::GAME_OVER) {
		if (_score > _highScore) {
			_highScore = _score;
		}
		_score = 0;
		_pausedTime = SDL_GetTicks() - _startTime;
		_state = State::ENDED;
	}
}
