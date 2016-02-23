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
#include "StateManager.h"

OptionsMenuState::OptionsMenuState() :
				_p1keys(ConfigHandler::getInstance().getKeyConfig(1)),
				_p2keys(ConfigHandler::getInstance().getKeyConfig(2)) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, 640, 480);
	_currentMenu = new OptionsMenu;
}

OptionsMenuState::~OptionsMenuState() {
	delete _currentMenu;
	SDL_DestroyTexture(_texture);
}

void OptionsMenuState::tick() {
	_currentMenu->handleInput();
}

SDL_Texture* OptionsMenuState::render() {
	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_RenderClear(_SDLRenderer);
	_currentMenu->render();

	return _texture;
}

void OptionsMenuState::goBack() {
	if (_menuStack.empty()) {
		StateManager::getInstance().returnToTile();
	}
}
