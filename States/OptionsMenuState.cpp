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
    _currentMenu = std::make_unique<OptionsMenu>(*this);
}

OptionsMenuState::~OptionsMenuState() {
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
        _currentMenu = std::move(_menuStack.back());
        _menuStack.pop_back();
    }
}

void OptionsMenuState::configurePlayerKeys(int player) {
    pushMenu(std::make_unique<KeyConfigMenu>(*this, player));
}

void OptionsMenuState::pushMenu(std::unique_ptr<Menu> menu) {
    _menuStack.push_back(std::move(_currentMenu));
    _currentMenu = std::move(menu);
}
