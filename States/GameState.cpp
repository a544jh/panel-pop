/*
 * GameState.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "GameState.h"

#include <SDL2/SDL_scancode.h>

#include "../Game/BoardEventHandler.h"
#include "../Game/GameEventHandler.h"
#include "../InputManager.h"
#include "../Menus/PauseMenu.h"
#include "StateManager.h"

GameState::GameState() :
				_renderer(_game),
				_game(new GameEventHandler(),
						new BoardEventHandler(_renderer, 0),
						new BoardEventHandler(_renderer, 1)),
				_p1keys(StateManager::getInstance().getP1keys()),
				_p2keys(StateManager::getInstance().getP2keys()),
				_kbc(_game._board0, _p1keys),
				_kbc2(_game._board1, _p2keys) {
}

void GameState::tick() {
	InputManager& input = InputManager::getInstance();
	if (input.keyDown(SDL_SCANCODE_5)) {
		_game.inputTogglePause();
	}
	if (input.keyPressed(SDL_SCANCODE_K)) {
		_game.inputAdvanceTick();
	}
	if (input.keyDown(SDL_SCANCODE_ESCAPE)) {
		_game.reset();
	}
	if (_game.getState() == Game::State::RUNNING) {
		_kbc.tick();
		_kbc2.tick();
	} else if (_game.getState() == Game::State::PAUSED) {
		//send input to pause menu instead
		PauseMenu& menu = _game.getPauseMenu();
		menu.handleInput();
	} else if (_game.getState() == Game::State::ENDED) {
		//TODO: change to any key and add timeout..?
		if (input.keyDown(SDL_SCANCODE_5)) {
			_game.reset();
		}
	}
	_game.tick();
	_renderer.tick();
}

SDL_Texture* GameState::render() {
	return _renderer.renderGame();
}

GameState::~GameState() {
}

void GameState::goBack() {
}
