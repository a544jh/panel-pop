/*
 * OptionsState.cpp
 *
 *  Created on: 14.2.2016
 *      Author: axelw
 */

#include "OptionsMenuState.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>

#include "../Config/ConfigHandler.h"
#include "../Menus/OptionsMenu.h"
#include "../Menus/KeyConfigMenu.h"
#include "StateManager.h"

OptionsMenuState::OptionsMenuState() {
    _texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET, 640, 480);
    _currentMenu = new OptionsMenu(*this);
}

OptionsMenuState::~OptionsMenuState() {
    delete _currentMenu;
    SDL_DestroyTexture(_texture);
}

void OptionsMenuState::tick() {
    _currentMenu->handleInput();
}

SDL_Texture *OptionsMenuState::render() {
    SDL_SetRenderTarget(_SDLRenderer, _texture);
    SDL_RenderClear(_SDLRenderer);
    _currentMenu->render();

    return _texture;
}

void OptionsMenuState::goBack() {
    if (_menuStack.empty()) {
        StateManager::getInstance().returnToTitle();
    } else {
        delete _currentMenu;
        _currentMenu = _menuStack.back();
        _menuStack.pop_back();
    }
}

void OptionsMenuState::configurePlayerKeys(int player) {
    pushMenu(new KeyConfigMenu(*this, player));
}

void OptionsMenuState::pushMenu(Menu *menu) {
    _menuStack.push_back(_currentMenu);
    _currentMenu = menu;
}
