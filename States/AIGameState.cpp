
/* 
 * File:   AIGameState.cpp
 * Author: axel
 * 
 * Created on September 26, 2016, 11:18 PM
 */

#include "AIGameState.h"

#include "../AI/AIBoardController.h"
#include "../Game/BoardEventHandler.h"
#include "../Game/GameEventHandler.h"
#include "StateManager.h"
#include "../Game/KeyboardController.h"
#include "../Game/VsGame.h"
#include "../Rendering/VsGameRenderer.h"

AIGameState::AIGameState() {
    _game = new VsGame(new GameEventHandler(),
            new BoardEventHandler(*_gameRenderer, 0),
            new BoardEventHandler(*_gameRenderer, 1));

    _gameRenderer = new VsGameRenderer((VsGame&) * _game);

    _playerBoardController = new KeyboardController(_game->getBoard(0), StateManager::getInstance().getP1keys());

    _opponentBoardcontollers.push_back(new AIBoardController(_game->getBoard(1)));
}


