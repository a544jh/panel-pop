/*
 * Game.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include "VsGame.h"

#include <SDL2/SDL_timer.h>

#include "BoardEventHandler.h"
#include "GameEventHandler.h"
#include "GarbageBlock.h"

VsGame::VsGame(GameEventHandler* geh) :
Game(geh),
_board0(this),
_board1(this),
_p1MatchPoints(0),
_p2MatchPoints(0),
_p1Points(0),
_p2Points(0) {
}

void VsGame::handleEnd() {
    if (_board0.getState() == Board::GAME_OVER
            || _board1.getState() == Board::GAME_OVER) {
        if (_board0.getState() == Board::RUNNING) {
            _board0.win();
            ++_p1MatchPoints;
        } else if (_board1.getState() == Board::RUNNING) {
            _board1.win();
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

void VsGame::tick() {

    if (_board0.getState() == Board::COUNTDOWN) {
        _eventHandler->countdown(getTime());
    }

    if (_state == State::RUNNING) {
        ++_ticksRun;
        handleGarbageSpawning(_board0, _board1);
        handleGarbageSpawning(_board1, _board0);
        _board0.tick();
        _board1.tick();

        if (!_panic && (_board0.isPanic() || _board1.isPanic())) {
            _panic = true;
            _eventHandler->panicBegin();
        }
        if (_panic && ((!_board0.isPanic() && !_board1.isPanic()))) {
            _panic = false;
            _eventHandler->panicEnd();
        }

        handleEnd();
        _eventHandler->tickEnd();
    }
}

int VsGame::getP1MatchPoints() const {
    return _p1MatchPoints;
}

int VsGame::getP1Points() const {
    return _p1Points;
}

int VsGame::getP2MatchPoints() const {
    return _p2MatchPoints;
}

int VsGame::getP2Points() const {
    return _p2Points;
}

Board& VsGame::getBoard(int id) {
    switch (id) {
        case 0:
            return _board0;
            break;
        default:
            return _board1;
            break;
    }
}

void VsGame::handleGarbageSpawning(Board& b1, Board& b2) {

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

void VsGame::reset() {
    _state = State::RUNNING;
    _startTime = SDL_GetTicks();
    _board0 = _board0.reset();
    _board1 = _board1.reset();
    _panic = false;
    _eventHandler->gameReset();
}

VsGame::~VsGame() {
    delete _board0.getEventHandler();
    delete _board1.getEventHandler();
}
