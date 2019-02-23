
/* 
 * File:   GameState.cpp
 * Author: axel
 * 
 * Created on September 26, 2016, 10:16 PM
 */

#include "GameState.h"
#include "../InputManager.h"

void GameState::tick() {
    InputManager& input = InputManager::getInstance();
    if (input.keyDown(SDL_SCANCODE_5)) {
        _game->inputTogglePause();
    }
    if (input.keyDown(SDL_SCANCODE_ESCAPE)) {
        _game->reset();
    }
    if (_game->getState() == Game::State::RUNNING) {
        _playerBoardController->tick();
        for (BoardController* controller : _opponentBoardcontollers) {
            controller->tick();
        }
    } else if (_game->getState() == Game::State::PAUSED) {
        //send input to pause menu instead
        PauseMenu& menu = _game->getPauseMenu();
        menu.handleInput();
    } else if (_game->getState() == Game::State::ENDED) {
        //TODO: change to any key and add timeout..?
        if (input.keyDown(SDL_SCANCODE_5)) {
            _game->reset();
        }
    }
    _game->tick();
    _gameRenderer->tick();
}

SDL_Texture* GameState::render() {
    return _gameRenderer->renderGame();
}

GameState::~GameState() {
    delete _game;
    delete _gameRenderer;
    delete _playerBoardController;
    for (BoardController* bc : _opponentBoardcontollers) {
        delete bc;
    }
}

void GameState::goBack() {

}


