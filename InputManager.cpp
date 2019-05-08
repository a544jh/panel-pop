/*
 * InputManager.cpp
 *
 *  Created on: Aug 19, 2015
 *      Author: axel
 */

#include "InputManager.h"
#include "InputEvents/InputEvent.h"
#include "InputEvents/JoyButton.h"
#include "InputEvents/KeyboardKey.h"
#include "InputEvents/JoyHat.h"
#include "InputEvents/JoyAxisDirection.h"
#include <SDL2/SDL.h>
#include <string.h>

InputManager::InputManager() :
    _quit(false),
    _keys_len(0),
    _inputDownEvent(),
    _prevInputDownEvent(),
    _joysticks(),
    _joyIdWithinDirection(-1),
    _joyAxisWithinDirection(-1) {
    _keys = SDL_GetKeyboardState(&_keys_len);
    _prevKeys = new uint8_t[_keys_len];
}

InputManager &InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

InputConfig InputManager::defaultMenuConfig = InputConfig(new KeyboardKey(SDL_SCANCODE_UP),
                                                          new KeyboardKey(SDL_SCANCODE_DOWN),
                                                          new KeyboardKey(SDL_SCANCODE_LEFT),
                                                          new KeyboardKey(SDL_SCANCODE_RIGHT),
                                                          new KeyboardKey(SDL_SCANCODE_X),
                                                          new KeyboardKey(SDL_SCANCODE_Z),
                                                          new KeyboardKey(SDL_SCANCODE_ESCAPE));
void InputManager::poll() {
    _keys = SDL_GetKeyboardState(&_keys_len);
    memcpy(_prevKeys, _keys, sizeof(uint8_t) * _keys_len);

    _prevInputDownEvent = _inputDownEvent;
    _inputDownEvent.type = SDL_FIRSTEVENT; // effectively set the event to "null"

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        filterInputDownEvent(e);
        if (e.type == SDL_QUIT) {
            _quit = true;
        } else if (e.type == SDL_JOYBUTTONDOWN) {
            //printf("Joy %d button %d pressed \n", e.jbutton.which, e.jbutton.button);
        } else if (e.type == SDL_JOYHATMOTION) {
            //printf("Joy %d hat %d pressed \n", e.jhat.which, e.jhat.value);
        } else if (e.type == SDL_JOYAXISMOTION) {
            //printf("Joy %d axis %d position %d \n", e.jaxis.which, e.jaxis.axis, e.jaxis.value);
        }
    }

    defaultMenuConfig.updateState();
}

void InputManager::filterInputDownEvent(const SDL_Event &e) {
    if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_KEYDOWN) {
        _inputDownEvent = e;
    }
    // filter to "key down" events: e.g. only when joystick "enters" axis direction...
    if (e.type == SDL_JOYAXISMOTION) {

        bool joyWithinDirection =
            (e.jaxis.value > JoyAxisDirection::AXIS_THRESHOLD || e.jaxis.value < -JoyAxisDirection::AXIS_THRESHOLD);

        if (joyWithinDirection) {
            if (_joyIdWithinDirection == -1) {
                _joyIdWithinDirection = e.jaxis.which;
                _joyAxisWithinDirection = e.jaxis.axis;
                _inputDownEvent = e;
            }
        } else {
            if (_joyIdWithinDirection == e.jaxis.which && _joyAxisWithinDirection == e.jaxis.axis) {
                _joyIdWithinDirection = -1;
            }
        }

    }

    if (e.type == SDL_JOYHATMOTION) {
        if (e.jhat.value != SDL_HAT_CENTERED) {
            _inputDownEvent = e;
        }
    }

}

InputEvent *InputManager::getInputDownEvent() const {
    const SDL_Event &e = _inputDownEvent;
    if (e.type == SDL_JOYBUTTONDOWN) {
        return new JoyButton(e.jbutton.which, e.jbutton.button);
    } else if (e.type == SDL_KEYDOWN) {
        return new KeyboardKey(e.key.keysym.scancode);
    } else if (e.type == SDL_JOYHATMOTION) {
        return new JoyHat(e.jhat.which, e.jhat.hat, e.jhat.value);
    } else if (e.type == SDL_JOYAXISMOTION) {
        JoyAxisDirection::Direction d;
        if (e.jaxis.value > JoyAxisDirection::AXIS_THRESHOLD) {
            d = JoyAxisDirection::POSITIVE;
        } else if (e.jaxis.value < -JoyAxisDirection::AXIS_THRESHOLD) {
            d = JoyAxisDirection::NEGATIVE;
        } else {
            return nullptr;
        }
        return new JoyAxisDirection(e.jaxis.which, e.jaxis.axis, d);
    }
    return nullptr;
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
    return (_prevInputDownEvent.type == SDL_FIRSTEVENT && _inputDownEvent.type != SDL_FIRSTEVENT);
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
        if (SDL_JoystickInstanceID(joystick) == id) {
            return joystick;
        }
    }
    return nullptr;
}
