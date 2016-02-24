/*
 * InputManager.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: axel
 */

#include "InputManager.h"
#include <SDL2/SDL.h>

InputManager::InputManager() :
				_quit(false) {
}

InputManager& InputManager::getInstance() {
	static InputManager instance;
	return instance;
}

void InputManager::poll() {
	_prevKeys = _keys;
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			_quit = true;
		} else if (e.type == SDL_KEYDOWN) {
			_keys[e.key.keysym.scancode] = true;
		} else if (e.type == SDL_KEYUP) {
			_keys[e.key.keysym.scancode] = false;
		}
	}
}

bool InputManager::keyDown(int key) {
	return !_prevKeys[key] && _keys[key];
}

bool InputManager::keyUp(int key) {
	return _prevKeys[key] && !_keys[key];
}

bool InputManager::keyPressed(int key) {
	return _keys[key];
}

bool InputManager::anyKeyDown() {
	for (int i = 0; i < KEYBOARD_SIZE; ++i) {
		if (!_prevKeys[i] && _keys[i]) {
			return true;
		}
	}
	return false;
}

int InputManager::getKeyDown() {
	for (int i = 0; i < KEYBOARD_SIZE; ++i) {
		if (!_prevKeys[i] && _keys[i]) {
			return i;
		}
	}
	return 0;
}
