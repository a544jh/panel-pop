/*
 * Game.cpp
 *
 *  Created on: 24.2.2016
 *      Author: axelw
 */

#include "Game.h"

#include <SDL2/SDL_timer.h>

#include "GameEventHandler.h"

Game::Game(GameEventHandler* geh) :
_eventHandler(geh),
_state(State::RUNNING),
_ticksRun(0),
_panic(false),
_startTime(SDL_GetTicks()),
_pausedTime(0),
_pauseMenu(*this) {
    _eventHandler->gameReset();
}

Game::~Game() {
    delete _eventHandler;
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

const bool Game::isPaused() const {
    return _state == State::PAUSED;
}

bool Game::isPanic() const {
    return _panic;
}

uint32_t Game::getPausedTime() const {
    return _pausedTime;
}

int Game::getTicksRun() const {
    return _ticksRun;
}

Game::State Game::getState() const {
    return _state;
}

PauseMenu& Game::getPauseMenu() {
    return _pauseMenu;
}

uint32_t Game::getStartTime() const {
    return _startTime;
}
