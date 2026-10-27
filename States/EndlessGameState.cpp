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
    _game = std::make_unique<EndlessGame>();
    _gameRenderer = std::make_unique<EndlessGameRenderer>((EndlessGame &) *_game);
    _game->getBoard(0).setEventHandler(std::make_unique<BoardEventHandler>(*_gameRenderer, 0));
    _playerBoardController = std::make_unique<KeyboardController>(_game->getBoard(0),
                                                                  StateManager::getInstance().getKeys(1));
}


