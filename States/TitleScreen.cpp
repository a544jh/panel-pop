/*
 * TitleScreen.cpp
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#include "TitleScreen.h"

#include "../InputManager.h"
#include "StateManager.h"

TitleScreen::TitleScreen() : _tr(){
}

TitleScreen::~TitleScreen() {
}

SDL_Texture* TitleScreen::render() {
	return _tr.render();
}

void TitleScreen::tick(){
	InputManager& input = InputManager::getInstance();
	if(input.anyKeyDown()){
		StateManager::getInstance().startGame();
	}
}
