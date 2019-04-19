 /*
 * EndlessGame.cpp
 *
 *  Created on: 24.2.2016
 *      Author: axelw
 */

#include "EndlessGame.h"

#include <SDL2/SDL_timer.h>

#include "../Config/ConfigHandler.h"
#include "BoardEventHandler.h"
#include "GameEventHandler.h"

class BoardEventHandler;

EndlessGame::EndlessGame() :
Game(),
_board(),
_highScore(ConfigHandler::getInstance().getEndlessHighScore()) {

}

EndlessGame::~EndlessGame() {
    delete _board.getEventHandler();
}

void EndlessGame::reset() {
    _state = State::RUNNING;
    _startTime = SDL_GetTicks();
    _board = _board.reset();
    _panic = false;
    _lastContdownMS = 0;
    _eventHandler->gameReset();
}

void EndlessGame::tick() {

    if (_board.getState() == Board::COUNTDOWN) {
        if (_lastContdownMS == 0) {
            _eventHandler->countdown(3);
        }
        uint32_t time = getTime();
        int timeSinceCountdown = time - _lastContdownMS;

        if (timeSinceCountdown >= COUNTDOWN_STEP_MS) {
            _board.advanceCountdownState();
            _lastContdownMS = time;
            _eventHandler->countdown(_board.getCountdownState());
        }
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

        if (_board.getScore() > _highScore) {
            _highScore = _board.getScore();
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

void EndlessGame::handleEnd() {
    if (_board.getState() == Board::GAME_OVER) {

        ConfigHandler::getInstance().setEndlessHighScore(_highScore);
        ConfigHandler::getInstance().saveConfig();

        _pausedTime = SDL_GetTicks() - _startTime;
        _state = State::ENDED;
    }
}
