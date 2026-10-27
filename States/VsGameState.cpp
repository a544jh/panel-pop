/*
 * GameState.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "VsGameState.h"

#include "../Game/BoardEventHandler.h"
#include "../Game/GameEventHandler.h"
#include "StateManager.h"
#include "../Game/KeyboardController.h"
#include "../Game/VsGame.h"
#include "../Rendering/VsGameRenderer.h"

VsGameState::VsGameState() {

    _game = std::make_unique<VsGame>();

    _gameRenderer = std::make_unique<VsGameRenderer>((VsGame &) *_game);

    _game->getBoard(0).setEventHandler(std::make_unique<BoardEventHandler>(*_gameRenderer, 0));
    _game->getBoard(1).setEventHandler(std::make_unique<BoardEventHandler>(*_gameRenderer, 1));

    _playerBoardController = std::make_unique<KeyboardController>(_game->getBoard(0), StateManager::getInstance().getKeys(1));

    _opponentBoardcontollers.push_back(std::make_unique<KeyboardController>(_game->getBoard(1),
                                                                            StateManager::getInstance().getKeys(2)));
}

