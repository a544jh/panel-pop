/*
 * StateManager.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "StateManager.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <iomanip>
#include <sstream>

#include "../Config/KeyboardControllerConfig.h"
#include "GameState.h"
#include "TitleScreen.h"

StateManager::StateManager() :
				_currentState(nullptr),
				SDL(SDLContext::getInstance()),
				input(InputManager::getInstance()),
				_lastTick(0),
				_frameTime(16),
				_running(true),
				_framesRun(0),
				_startTime(SDL_GetTicks()),
				_lastFrame(0),
				_avgFps(0),
				_showFps(false) {
	_currentState = new TitleScreen();
}

StateManager& StateManager::getInstance() {
	static StateManager instance;
	return instance;
}

void StateManager::run() {
	bool tick;
	bool render;
	while (_running) {
		tick = (SDL_GetTicks() - _lastTick >= _frameTime);
		render = (SDL_GetTicks() - _lastFrame >= 5);
		//200 fps cap
		if (tick) {
			input.poll();
			if (input._quit) {
				//TODO: State transitions?
				_running = false;
				break;
			}
			if (input.keyPressed(SDL_SCANCODE_LALT)
					&& input.keyDown(SDL_SCANCODE_RETURN)) {
				SDL.toggleFullscreen();
			}
			if (input.keyPressed(SDL_SCANCODE_LCTRL)
					&& input.keyDown(SDL_SCANCODE_F)) {
				_showFps = !_showFps;
			}
			_currentState->tick();
		}
		if (render) {
			SDL_Texture* t = _currentState->render();

			if (_showFps) {
				SDL_SetRenderTarget(SDL.getRenderer(), t);
				showFps();
			}

			SDL.renderTextureToWindow(t);
			_lastFrame = SDL_GetTicks();
			++_framesRun;
		}
		if (tick) {
			_lastTick = SDL_GetTicks();
		}

	}
}

void StateManager::switchToState(State* state) {
	delete _currentState;
	_currentState = state;
}

void StateManager::startGame() {
	//TODO:different game configurations
	KeyboardControllerConfig c = { SDL_SCANCODE_UP, SDL_SCANCODE_DOWN,
			SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_Z,
			SDL_SCANCODE_X };
	//KeyboardControllerConfig c = {SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT,SDL_SCANCODE_X,SDL_SCANCODE_Z};
	KeyboardControllerConfig c2 = { SDL_SCANCODE_R, SDL_SCANCODE_F,
			SDL_SCANCODE_D, SDL_SCANCODE_G, SDL_SCANCODE_T, SDL_SCANCODE_Y };
	//passed as reference but the KeyboardController copies it :P

	switchToState(new GameState(c, c2));
}

float StateManager::getAvgFps() const {
	return _avgFps;
}

void StateManager::showFps() {
	std::ostringstream ss;
	_avgFps = _framesRun / ((SDL_GetTicks() - _startTime) / 1000.f);
	if (_avgFps > 2000000) {
		_avgFps = 0;
	}
	ss << std::setprecision(5) << _avgFps;
	SDL.renderText(ss.str(), { 0, 0, 0 }, SDL._psFont, 0, 0);
}
