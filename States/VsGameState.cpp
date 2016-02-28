/*
 * GameState.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "VsGameState.h"

#include <SDL2/SDL_scancode.h>

#include "../Game/BoardEventHandler.h"
#include "../Game/GameEventHandler.h"
#include "../InputManager.h"
#include "../Menus/PauseMenu.h"
#include "StateManager.h"

VsGameState::VsGameState() :
				_vsGame(new GameEventHandler(),
						new BoardEventHandler(_renderer, 0),
						new BoardEventHandler(_renderer, 1)),
				_renderer(_vsGame),
				_p1keys(StateManager::getInstance().getP1keys()),
				_p2keys(StateManager::getInstance().getP2keys()),
				_kbc(_vsGame.getBoard(0), _p1keys),
				_kbc2(_vsGame.getBoard(1), _p2keys) {
}

void VsGameState::tick() {
	InputManager& input = InputManager::getInstance();
	if (input.keyDown(SDL_SCANCODE_5)) {
		_vsGame.inputTogglePause();
	}
	if (input.keyDown(SDL_SCANCODE_ESCAPE)) {
		_vsGame.reset();
	}
	if (_vsGame.getState() == Game::State::RUNNING) {
		_kbc.tick();
		_kbc2.tick();
	} else if (_vsGame.getState() == Game::State::PAUSED) {
		//send input to pause menu instead
		PauseMenu& menu = _vsGame.getPauseMenu();
		menu.handleInput();
	} else if (_vsGame.getState() == Game::State::ENDED) {
		//TODO: change to any key and add timeout..?
		if (input.keyDown(SDL_SCANCODE_5)) {
			_vsGame.reset();
		}
	}
	_vsGame.tick();
	_renderer.tick();
}

SDL_Texture* VsGameState::render() {
	return _renderer.renderGame();
}

VsGameState::~VsGameState() {
}

void VsGameState::goBack() {
}
