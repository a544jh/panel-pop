
/* 
 * File:   DemoGameState.cpp
 * Author: axel
 * 
 * Created on October 5, 2016, 9:36 PM
 */

#include "DemoGameState.h"
#include "StateManager.h"
#include "../InputManager.h"
#include "../Game/VsGame.h"
#include "../Rendering/VsGameRenderer.h"
#include "../AI/AIBoardController.h"
#include "../AI/AIStrategy.h"
#include "../Config/ConfigHandler.h"
#include "../Game/GameEventHandler.h"

DemoGameState::DemoGameState() {

    _game = new VsGame();

    _gameRenderer = new VsGameRenderer((VsGame &) *_game);

    _game->getBoard(0).setEventHandler(new BoardEventHandler(*_gameRenderer, 0));
    _game->getBoard(1).setEventHandler(new BoardEventHandler(*_gameRenderer, 1));

    // Player 1 plays the "AI strategy" from the options menu (the one "VS AI"
    // puts up), player 2 the "P2 AI" one, which follows "AI strategy" until
    // it is set - so by default the selected AI plays itself.
    ConfigHandler &config = ConfigHandler::getInstance();
    _playerBoardController = new AIBoardController(_game->getBoard(0),
                                                   AIStrategies::create(config.getAiStrategy()));

    _opponentBoardcontollers.push_back(new AIBoardController(_game->getBoard(1),
                                                             AIStrategies::create(config.getDemoP2Strategy())));
}

void DemoGameState::tick() {

    InputManager &input = InputManager::getInstance();
    if (input.anyKeyDown()) {
        Mix_HaltMusic();
        StateManager::getInstance().returnToTitle();
        return;
    }
    if (_game->getState() == Game::State::RUNNING) {
        _playerBoardController->tick();
        for (BoardController *controller : _opponentBoardcontollers) {
            controller->tick();
        }
    } else if (_game->getState() == Game::State::ENDED) {
        Mix_HaltMusic();
        StateManager::getInstance().returnToTitle();
        return;
    }
    _game->tick();
    _gameRenderer->tick();

}

SDL_Texture *DemoGameState::render() {
    SDL_Texture *texture = _gameRenderer->renderGame();
    SDL_SetRenderTarget(SDLContext::getInstance().getRenderer(), texture);
    if (SDL_GetTicks() % 1000 < 500) {
        SDLContext::getInstance().renderText("PUSH ANY KEY",
                                             {255, 255, 255},
                                             SDLContext::getInstance()._fontPs,
                                             440,
                                             450);
    }
    return texture;
}

DemoGameState::~DemoGameState() {
}

