/*
 * StateManager.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "StateManager.h"
#include "GameState.h"
#include "../SDLContext.h"

StateManager::StateManager() :
		_currentState(nullptr), _lastTick(0), _running(true), SDL(
				SDLContext::getInstance()), input(InputManager::getInstance()), _frameTime(
				16) {
	_currentState = new GameState();
}

StateManager& StateManager::getInstance() {
	static StateManager instance;
	return instance;
}

void StateManager::run() {
	bool tick;
	while (_running) {
		tick = (SDL_GetTicks() - _lastTick >= _frameTime);
		if (tick) {
			input.poll();
			if (input._quit) {
				//TODO: State transitions?
				_running = false;
				break;
			}
			_currentState->tick();
		}
		SDL_Texture* t = _currentState->render();
		SDL.renderTextureToWindow(t);
		if(tick){
			_lastTick = SDL_GetTicks();
		}
	}
}

