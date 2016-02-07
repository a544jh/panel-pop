/*
 * GameState.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "GameState.h"

#include <SDL2/SDL_scancode.h>
#include <iostream>

#include "../InputManager.h"
#include "../Menus/PauseMenu.h"

GameState::GameState(KeyboardControllerConfig& c1, KeyboardControllerConfig& c2) :
				_game(),
				_renderer(_game),
				_kbc(_game._board, c1),
				_kbc2(_game._board2, c2),
				_p1keys(c1),
				_p2keys(c2) {
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
		if (input.keyDown(_p1keys.down) || input.keyDown(_p2keys.down)) {
			menu.inputDown();
		}
		if (input.keyDown(_p1keys.up) || input.keyDown(_p2keys.up)) {
			menu.inputUp();
		}
		if (input.keyDown(_p1keys.swap) || input.keyDown(_p2keys.swap)) {
			menu.inputEnter();
		}
		if (input.keyDown(_p1keys.raiseStack)
				|| input.keyDown(_p2keys.raiseStack)) {
			menu.inputCancel();
		}
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

