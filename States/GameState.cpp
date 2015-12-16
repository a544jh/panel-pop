/*
 * GameState.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "GameState.h"

#include <SDL2/SDL.h>
#include "../InputManager.h"
GameState::GameState() :
		game(), renderer(game), kbc(game._board) {
}

void GameState::tick() {
	InputManager& input = InputManager::getInstance();
	if (input._keys[SDL_SCANCODE_P] && !input._prevKeys[SDL_SCANCODE_P]) {
		game.inputTogglePause();
	}
	if (input._keys[SDL_SCANCODE_R] && !input._prevKeys[SDL_SCANCODE_R]) {
			game.reset();
		}
	if (!game.isPaused()) {
		kbc.tick();
	}
	game.tick();
	renderer.tick();
}

SDL_Texture* GameState::render() {
	return renderer.renderGame();
}

GameState::~GameState() {
	// TODO Auto-generated destructor stub
}

