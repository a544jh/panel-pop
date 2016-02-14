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
#include "../InputManager.h"
#include "../Menus/KeyConfigMenu.h"

OptionsMenuState::OptionsMenuState() :
				_p1keys(ConfigHandler::getInstance().getKeyConfig(1)),
				_p2keys(ConfigHandler::getInstance().getKeyConfig(2)),
				_waitingForKey(false) {
	_texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, 640, 480);
	_currentMenu = new KeyConfigMenu(*this, 1);
}

OptionsMenuState::~OptionsMenuState() {
	delete _currentMenu;
	SDL_DestroyTexture(_texture);
}

void OptionsMenuState::tick() {
	InputManager& input = InputManager::getInstance();
	if (_waitingForKey) {
		//waiting for key config input :P
		int pressed = input.getKeyDown();
		if (pressed != 0) {
			KeyConfigMenu* kcfgm = dynamic_cast<KeyConfigMenu*>(_currentMenu);
			kcfgm->recieveKey(pressed);
			_waitingForKey = false;
		}
	} else {
		if (input.keyDown(_p1keys.down) || input.keyDown(_p2keys.down)) {
			_currentMenu->inputDown();
		}
		if (input.keyDown(_p1keys.up) || input.keyDown(_p2keys.up)) {
			_currentMenu->inputUp();
		}
		if (input.keyDown(_p1keys.swap) || input.keyDown(_p2keys.swap)) {
			_currentMenu->inputEnter();
		}
		if (input.keyDown(_p1keys.raiseStack)
				|| input.keyDown(_p2keys.raiseStack)) {
			_currentMenu->inputCancel();
		}
	}
}

SDL_Texture* OptionsMenuState::render() {
	SDL_SetRenderTarget(_SDLRenderer, _texture);
	SDL_RenderClear(_SDLRenderer);
	_currentMenu->render();

	if (_waitingForKey) {
		_SDLContext.renderText("--press key--", { 255, 255, 255 },
				_SDLContext._fontPs, 0, 300);
	}

	return _texture;
}

void OptionsMenuState::getKey() {
	_waitingForKey = true;
}
