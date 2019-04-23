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

#include "../InputEvents/JoyHat.h"
#include "../InputEvents/JoyButton.h"

#include "../InputManager.h"
#include "../SDLContext.h"
#include "../Config/ConfigHandler.h"
#include "EndlessGameState.h"
#include "TitleScreen.h"
#include "FadeTransition.h"
#include "../InputEvents/JoyAxisDirection.h"

StateManager::StateManager() :
    _currentState(nullptr),
    _newState(nullptr),
    _switchState(false),
    SDL(SDLContext::getInstance()),
    input(InputManager::getInstance()),
    _lastTick(0),
    _frameTime(16),
    _running(true),
    _framesRun(0),
    _startTime(SDL_GetTicks()),
    _lastFrame(0),
    _avgFps(0),
    _showFps(false),
    _inputConfigs{ConfigHandler::getInstance().getKeyConfig(1),
        /*InputConfig(new JoyHat(0, 0, SDL_HAT_UP),
                    new JoyHat(0, 0, SDL_HAT_DOWN),
                    new JoyHat(0, 0, SDL_HAT_LEFT),
                    new JoyHat(0, 0, SDL_HAT_RIGHT),
                    new JoyButton(0, 0),
                    new JoyButton(0, 1))*/
        /*InputConfig(new JoyAxisDirection(0, 1, JoyAxisDirection::NEGATIVE),
                    new JoyAxisDirection(0, 1, JoyAxisDirection::POSITIVE),
                    new JoyAxisDirection(0, 0, JoyAxisDirection::NEGATIVE),
                    new JoyAxisDirection(0, 0, JoyAxisDirection::POSITIVE),
                    new JoyButton(0, 1),
                    new JoyButton(0, 2)),*/
        InputConfig(new KeyboardKey(SDL_SCANCODE_UP),
                    new KeyboardKey(SDL_SCANCODE_DOWN),
                    new KeyboardKey(SDL_SCANCODE_LEFT),
                    new KeyboardKey(SDL_SCANCODE_RIGHT),
                    new KeyboardKey(SDL_SCANCODE_X),
                    new KeyboardKey(SDL_SCANCODE_Z),
                    new KeyboardKey(SDL_SCANCODE_ESCAPE))} {
    _currentState = new TitleScreen();
}

StateManager &StateManager::getInstance() {
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
            for (auto &&config : _inputConfigs) {
                config.updateState();
            }
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
            SDL_Texture *t = _currentState->render();

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
        if (_switchState) {
            delete _currentState;
            _currentState = _newState;
            _switchState = false;
        }

    }
}

void StateManager::switchToState(State *state) {
    _newState = state;
    _switchState = true;
}

float StateManager::getAvgFps() const {
    return _avgFps;
}

State *StateManager::getCurrentState() const {
    return _currentState;
}

void StateManager::returnToTitle() {
    if (!dynamic_cast<FadeTransition *>(_currentState)) {
        switchToState(new FadeTransition([]() {
          return new TitleScreen;
        }));
    }
}

InputConfig &StateManager::getKeys(int player) {
    return _inputConfigs[player - 1];
}

void StateManager::setKeys(InputConfig keys, int player) {
    _inputConfigs[player - 1] = keys;
}

void StateManager::showFps() {
    std::ostringstream ss;
    _avgFps = _framesRun / ((SDL_GetTicks() - _startTime) / 1000.f);
    if (_avgFps > 2000000) {

        _avgFps = 0;
    }
    ss << std::setprecision(5) << _avgFps;
    SDL.renderText(ss.str(), {0, 0, 0}, SDL._fontPs, 0, 0);
}

void StateManager::goBack() {

    _currentState->goBack();
}

void StateManager::quit() {
    _running = false;
}
