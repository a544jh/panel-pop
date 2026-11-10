
/* 
 * File:   GameState.cpp
 * Author: axel
 * 
 * Created on September 26, 2016, 10:16 PM
 */

#include "GameState.h"

#include <iostream>

#include "../AI/AIBoardController.h"
#include "../AI/AIStrategy.h"
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

Game *GameState::getGame() const {
    return _game;
}

void GameState::setAiStrategy(const std::string &key, int index) {
    BoardController *target = nullptr;
    if (index == 0) {
        target = _playerBoardController;
    } else if (index > 0 && index - 1 < (int) _opponentBoardcontollers.size()) {
        target = _opponentBoardcontollers[index - 1];
    }
    AIBoardController *ai = dynamic_cast<AIBoardController *>(target);
    if (ai != nullptr) {
        ai->setStrategy(AIStrategies::create(key));
    } else if (target != nullptr) {
        // Not a computer controlled board (a KeyboardController): swap in an
        // AI, so a VS game can be played computer against computer. Used by
        // the debug console to benchmark strategies.
        BoardController *replacement =
            new AIBoardController(target->board(), AIStrategies::create(key));
        if (target == _playerBoardController) {
            delete _playerBoardController;
            _playerBoardController = replacement;
        } else {
            for (size_t i = 0; i < _opponentBoardcontollers.size(); ++i) {
                if (_opponentBoardcontollers[i] == target) {
                    delete _opponentBoardcontollers[i];
                    _opponentBoardcontollers[i] = replacement;
                    break;
                }
            }
        }
    } else {
        std::cout << "No such board." << std::endl;
    }
}

void GameState::setAiStrategy(const std::string &key) {
    for (BoardController *controller : _opponentBoardcontollers) {
        AIBoardController *ai = dynamic_cast<AIBoardController *>(controller);
        if (ai != nullptr) {
            ai->setStrategy(AIStrategies::create(key));
        }
    }
}

std::string GameState::aiStrategyName() const {
    for (BoardController *controller : _opponentBoardcontollers) {
        const AIBoardController *ai = dynamic_cast<const AIBoardController *>(controller);
        if (ai != nullptr) {
            return ai->strategyName();
        }
    }
    return "";
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


