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
		game(), renderer(game), kbc(game._board, c1),
		kbc2(game._board2, c2) {
}

void GameState::tick() {
	InputManager& input = InputManager::getInstance();
	if (input.keyDown(SDL_SCANCODE_5)) {
		game.inputTogglePause();
	}
	if (input.keyPressed(SDL_SCANCODE_K)) {
			game.inputAdvanceTick();
		}
	if (input.keyDown(SDL_SCANCODE_ESCAPE)) {
			game.reset();
		}
	if (!game.isPaused() || game.isAdvanceTick()) {
		kbc.tick();
		kbc2.tick();
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

