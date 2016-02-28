/*
 * EndlessGameState.cpp
 *
 *  Created on: Feb 28, 2016
 *      Author: axel
 */

#include "EndlessGameState.h"

#include <SDL2/SDL_scancode.h>

#include "../Game/BoardEventHandler.h"
#include "../Game/Game.h"
#include "../Game/GameEventHandler.h"
#include "../InputManager.h"
#include "../Menus/PauseMenu.h"
#include "StateManager.h"

EndlessGameState::EndlessGameState() :
				_endlessGame(new GameEventHandler(),
						new BoardEventHandler(_renderer, 0)),
				_renderer(_endlessGame),
				_kbc(_endlessGame.getBoard(0),
						StateManager::getInstance().getP1keys()) {

}

EndlessGameState::~EndlessGameState() {
}

void EndlessGameState::tick() {
	InputManager& input = InputManager::getInstance();
	if (input.keyDown(SDL_SCANCODE_5)) {
		_endlessGame.inputTogglePause();
	}
	if (input.keyDown(SDL_SCANCODE_ESCAPE)) {
		_endlessGame.reset();
	}
	if (_endlessGame.getState() == Game::State::RUNNING) {
		_kbc.tick();
	} else if (_endlessGame.getState() == Game::State::PAUSED) {
		//send input to pause menu instead
		PauseMenu& menu = _endlessGame.getPauseMenu();
		menu.handleInput();
	} else if (_endlessGame.getState() == Game::State::ENDED) {
		//TODO: change to any key and add timeout..?
		if (input.keyDown(SDL_SCANCODE_5)) {
			_endlessGame.reset();
		}
	}
	_endlessGame.tick();
	_renderer.tick();

}

SDL_Texture* EndlessGameState::render() {
	return _renderer.renderGame();
}

void EndlessGameState::goBack() {
}
