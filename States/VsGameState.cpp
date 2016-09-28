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

    _game = new VsGame(new GameEventHandler());

    _gameRenderer = new VsGameRenderer((VsGame&) * _game);

    _game->getBoard(0).setEventHandler(new BoardEventHandler(*_gameRenderer, 0));
    _game->getBoard(1).setEventHandler(new BoardEventHandler(*_gameRenderer, 1));

    _playerBoardController = new KeyboardController(_game->getBoard(0), StateManager::getInstance().getP1keys());

    _opponentBoardcontollers.push_back(new KeyboardController(_game->getBoard(1), StateManager::getInstance().getP2keys()));
}

VsGameState::~VsGameState() {
}

