/*
 * TitleScreen.cpp
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#include "TitleScreen.h"

#include "../InputManager.h"
#include "StateManager.h"

TitleScreen::TitleScreen() : _tr(*this){
}

TitleScreen::~TitleScreen() {
}

SDL_Texture* TitleScreen::render() {
	return _tr.render();
}

void TitleScreen::tick(){
	_menu.handleInput();
}

const MainMenu& TitleScreen::getMenu() const {
	return _menu;
}

void TitleScreen::goBack() {
}
