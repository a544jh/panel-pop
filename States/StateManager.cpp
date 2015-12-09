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
				SDLContext::getInstance()), input(InputManager::getInstance()) {
	_currentState = new GameState();
}

StateManager& StateManager::getInstance()
{
	static StateManager instance;
	return instance;
}

void StateManager::run() {
	while (_running) {
		input.poll();
		if (input._quit) {
			//TODO: State transitions?
			_running = false;
			break;
		}
		if (SDL_GetTicks() - _lastTick >= 16) {
			_currentState->tick();
			//TODO: Manage tickrate somewhere else?
			_lastTick = SDL_GetTicks();
		}
		SDL_Texture* t = _currentState->render();
		SDL.renderTextureToWindow(t);
	}
}

