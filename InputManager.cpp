/*
 * InputManager.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: axel
 */

#include "InputManager.h"
#include <SDL2/SDL.h>

InputManager::InputManager() :
_quit(false)
{}

InputManager& InputManager::getInstance()
{
	static InputManager instance;
	return instance;
}


void InputManager::poll() {
	SDL_Event e;
	while(SDL_PollEvent( &e ) != 0) {
		if(e.type == SDL_QUIT) {
			_quit = true;
		} else if (e.type == SDL_KEYDOWN) {
			_keys[e.key.keysym.scancode] = true;
		} else if (e.type == SDL_KEYUP) {
			_keys[e.key.keysym.scancode] = false;
		}
	}
}

