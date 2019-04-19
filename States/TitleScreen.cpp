/*
 * TitleScreen.cpp
 *
 *  Created on: 15.1.2016
 *      Author: axelw
 */

#include "TitleScreen.h"

#include "../InputManager.h"
#include "StateManager.h"
#include "DemoGameState.h"
#include "FadeTransition.h"

TitleScreen::TitleScreen() : _tr(*this), _idleTicks(0) {
}

TitleScreen::~TitleScreen() {
}

SDL_Texture *TitleScreen::render() {
    return _tr.render();
}

void TitleScreen::tick() {
    if (InputManager::getInstance().anyKeyDown()) {
        _idleTicks = 0;
    }
    if (++_idleTicks >= DEMO_TIMEOUT) {
        StateManager::getInstance().switchToState(new FadeTransition([]() {
          return new DemoGameState;
        }));
    }
    _menu.handleInput();
}

const MainMenu &TitleScreen::getMenu() const {
    return _menu;
}

void TitleScreen::goBack() {
}
