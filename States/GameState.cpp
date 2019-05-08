
/* 
 * File:   GameState.cpp
 * Author: axel
 * 
 * Created on September 26, 2016, 10:16 PM
 */

#include "GameState.h"
#include "../InputManager.h"
#include "StateManager.h"

void GameState::tick() {
    InputConfig conf1 = StateManager::getInstance().getKeys(1);
    InputConfig conf2 = StateManager::getInstance().getKeys(2);
    if (conf1.startDown() || conf2.startDown() || InputManager::defaultMenuConfig.startDown()) {
        _game->inputTogglePause();
    }
    if (false) {
        // TODO: enable in debug mode...
        _game->reset();
    }
    if (_game->getState() == Game::State::RUNNING) {
        _playerBoardController->tick();
        for (BoardController *controller : _opponentBoardcontollers) {
            controller->tick();
        }
    } else if (_game->getState() == Game::State::PAUSED) {
        //send input to pause menu instead
        PauseMenu &menu = _game->getPauseMenu();
        menu.handleInput();
    } else if (_game->getState() == Game::State::ENDED) {
        //TODO: change to any key and add timeout..?
        if (conf1.startDown() || conf2.startDown() || InputManager::defaultMenuConfig.startDown()) {
            _game->reset();
        }
    }
    _game->tick();
    _gameRenderer->tick();
}

SDL_Texture *GameState::render() {
    return _gameRenderer->renderGame();
}

GameState::~GameState() {
    delete _game;
    delete _gameRenderer;
    delete _playerBoardController;
    for (BoardController *bc : _opponentBoardcontollers) {
        delete bc;
    }
}

void GameState::goBack() {

}


