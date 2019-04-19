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

    _game = new VsGame();

    _gameRenderer = new VsGameRenderer((VsGame &) *_game);

    _game->getBoard(0).setEventHandler(new BoardEventHandler(*_gameRenderer, 0));
    _game->getBoard(1).setEventHandler(new BoardEventHandler(*_gameRenderer, 1));

    _playerBoardController = new KeyboardController(_game->getBoard(0), StateManager::getInstance().getKeys(1));

    _opponentBoardcontollers.push_back(new KeyboardController(_game->getBoard(1),
                                                              StateManager::getInstance().getKeys(2)));
}

VsGameState::~VsGameState() {
}

