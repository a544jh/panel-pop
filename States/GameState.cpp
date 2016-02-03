/*
 * GameState.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "GameState.h"
#include "../Config/KeyboardControllerConfig.h"
#include <SDL2/SDL.h>
#include "../InputManager.h"
GameState::GameState(KeyboardControllerConfig& c1, KeyboardControllerConfig& c2) :
		_game(), _renderer(_game), _kbc(_game._board, c1),
		_kbc2(_game._board2, c2) {
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
	if (!_game.isPaused() || _game.isAdvanceTick()) {
		_kbc.tick();
		_kbc2.tick();
	}
	_game.tick();
	_renderer.tick();
}

SDL_Texture* GameState::render() {
	return _renderer.renderGame();
}

GameState::~GameState() {
}

