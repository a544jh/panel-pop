/*
 * InputManager.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: axel
 */

#include "InputManager.h"
#include <SDL2/SDL.h>
#include <string.h>

InputManager::InputManager() :
    _quit(false), _keys_len(0), _joysticks() {
    _keys = SDL_GetKeyboardState(&_keys_len);
    _prevKeys = new uint8_t[_keys_len];
}

InputManager &InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

void InputManager::poll() {
    _keys = SDL_GetKeyboardState(&_keys_len);
    memcpy(_prevKeys, _keys, sizeof(uint8_t) * _keys_len);
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            _quit = true;
        } else if (e.type == SDL_JOYBUTTONDOWN) {
            printf("Joy %d button %d pressed \n", e.jbutton.which, e.jbutton.button);
        } else if (e.type == SDL_JOYHATMOTION) {
            printf("Joy %d hat %d pressed \n", e.jhat.which, e.jhat.value);
        } else if (e.type == SDL_JOYAXISMOTION) {
            //printf("Joy %d axis %d position %d \n", e.jaxis.which, e.jaxis.axis, e.jaxis.value);
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
    for (int i = 0; i < _keys_len; ++i) {
        if (!_prevKeys[i] && _keys[i]) {
            return true;
        }
    }
    return false;
}

int InputManager::getKeyDown() {
    for (int i = 0; i < _keys_len; ++i) {
        if (!_prevKeys[i] && _keys[i]) {
            return i;
        }
    }
    return 0;
}
void InputManager::detectJoysticks() {
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        _joysticks.push_back(SDL_JoystickOpen(i));
    }
}

SDL_Joystick *InputManager::getJoystick(SDL_JoystickID id) {
    for (auto joystick: _joysticks) {
        if (SDL_JoystickInstanceID(joystick) == id){
            return joystick;
        }
    }
}
