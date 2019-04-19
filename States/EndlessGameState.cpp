/*
 * EndlessGameState.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */

#include "EndlessGameState.h"

#include <SDL2/SDL_scancode.h>

#include "../Game/BoardEventHandler.h"
#include "../Game/GameEventHandler.h"
#include "StateManager.h"

EndlessGameState::EndlessGameState() {
    _game = new EndlessGame();
    _gameRenderer = new EndlessGameRenderer((EndlessGame &) *_game);
    _game->getBoard(0).setEventHandler(new BoardEventHandler(*_gameRenderer, 0));
    _playerBoardController = new KeyboardController(_game->getBoard(0),
                                                    StateManager::getInstance().getKeys(1));
}

EndlessGameState::~EndlessGameState() {
}


