/*
 * EndlessGameState.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */


#include "StateManager.h"
#include "EndlessGameState.h"

#include "../Game/BoardEventHandler.h"
#include "../Game/GameEventHandler.h"

#include <SDL2/SDL_scancode.h>


EndlessGameState::EndlessGameState() {
    _game = new EndlessGame(new GameEventHandler());
    _gameRenderer = new EndlessGameRenderer((EndlessGame &) * _game);
    _game->getBoard(0).setEventHandler(new BoardEventHandler(*_gameRenderer, 0));
    _playerBoardController = new KeyboardController(_game->getBoard(0),
            StateManager::getInstance().getP1keys());
}

EndlessGameState::~EndlessGameState() {
}


