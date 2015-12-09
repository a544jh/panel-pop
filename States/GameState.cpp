/*
 * GameState.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "GameState.h"

GameState::GameState() : game(), renderer(game) {
}

void GameState::tick() {
	game.tick();
	renderer.tick();
}

SDL_Texture* GameState::render() {
	return renderer.renderGame();
}

GameState::~GameState() {
	// TODO Auto-generated destructor stub
}

