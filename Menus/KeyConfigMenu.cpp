/*
 * KeyConfigMenu.cpp
 *
 *  Created on: 14.2.2016
 *      Author: axelw
 */

#include "KeyConfigMenu.h"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <string>
#include <vector>

#include "../Config/ConfigHandler.h"
#include "../InputManager.h"
#include "../SDLContext.h"
#include "MenuItem.h"
#include "../States/OptionsMenuState.h"

KeyConfigMenu::KeyConfigMenu(OptionsMenuState &state, int player) :
    _state(state),
    _player(player),
    _waitingForKey(false),
    _newKeyConfig(ConfigHandler::getInstance().getKeyConfig(player)) {

    addItem(
        MenuItem("Up",
                 [&]() {
                   _inputToSet = &_newKeyConfig._up;
                   _waitingForKey = true;
                 },
                 0, 0, MenuItem::OptionType::NONE));
    addItem(
        MenuItem("Down",
                 [&]() {
                   _inputToSet = &_newKeyConfig._down;
                   _waitingForKey = true;
                 },
                 0, 0, MenuItem::OptionType::NONE));
    addItem(
        MenuItem("Left",
                 [&]() {
                   _inputToSet = &_newKeyConfig._left;
                   _waitingForKey = true;
                 },
                 0, 0, MenuItem::OptionType::NONE));
    addItem(
        MenuItem("Right",
                 [&]() {
                   _inputToSet = &_newKeyConfig._right;
                   _waitingForKey = true;
                 },
                 0, 0, MenuItem::OptionType::NONE));
    addItem(
        MenuItem("Swap/Accept",
                 [&]() {
                   _inputToSet = &_newKeyConfig._swap;
                   _waitingForKey = true;
                 },
                 0, 0, MenuItem::OptionType::NONE));
    addItem(
        MenuItem("Raise stack/Cancel",
                 [&]() {
                   _inputToSet = &_newKeyConfig._raiseStack;
                   _waitingForKey = true;
                 },
                 0, 0, MenuItem::OptionType::NONE));
    addItem(
        MenuItem("Pause/Start",
                 [&]() {
                   _inputToSet = &_newKeyConfig._start;
                   _waitingForKey = true;
                 },
                 0, 0, MenuItem::OptionType::NONE));

    addItem(MenuItem("Apply", [&]() {
      ConfigHandler::getInstance().setKeyConfig(_newKeyConfig, _player);
      ConfigHandler::getInstance().saveConfig();
      _state.goBack();
    }));

}

KeyConfigMenu::~KeyConfigMenu() {
}

void KeyConfigMenu::setActiveInput(InputEvent *event) {
    _inputToSet->reset(event);
}

void KeyConfigMenu::render() const {

    int x = 0;
    int y = 0;

    for (unsigned i = 0; i < _items.size(); ++i) {
        y += 18;
        std::string text = _items.at(i).getName();
        if (_selection == i && (SDL_GetTicks() - _time) % 1000 < 500) {
            text = "\u25b6" + text;
        } else {
            text = " " + text;
        }
        _SDLContext.renderText(text, {0, 0, 0}, _SDLContext._fontPs, x, y);

        if (i != _items.size() - 1 && !(_waitingForKey && _selection == i)) {
            const char *name;

            switch (i) {
                case 0: name = _newKeyConfig._up->toString().c_str();
                    break;
                case 1: name = _newKeyConfig._down->toString().c_str();
                    break;
                case 2: name = _newKeyConfig._left->toString().c_str();
                    break;
                case 3: name = _newKeyConfig._right->toString().c_str();
                    break;
                case 4: name = _newKeyConfig._swap->toString().c_str();
                    break;
                case 5: name = _newKeyConfig._raiseStack->toString().c_str();
                    break;
                case 6: name = _newKeyConfig._start->toString().c_str();
                    break;
                default:
                    name = "";
            }

            _SDLContext.renderText(name, {0, 0, 0}, _SDLContext._fontPs,
                                   x + 350, y);
        } else if (_selection == i && _waitingForKey && (SDL_GetTicks() - _time) % 1000 < 500) {
            _SDLContext.renderText("--press key--", {0, 0, 0},
                                   _SDLContext._fontPs, x + 350, y);
        }
    }

}

void KeyConfigMenu::handleInput() {
    InputManager &input = InputManager::getInstance();
    if (_waitingForKey) {
        InputEvent *event = InputManager::getInstance().getInputDownEvent();
        if (input.keyDown(SDL_SCANCODE_ESCAPE)) {
            _waitingForKey = false;
        } else if (event != nullptr) {
            setActiveInput(event);
            inputDown();
            inputEnter();
        }
    } else if (input.keyDown(SDL_SCANCODE_ESCAPE)) {
        _state.goBack();
    } else {
        Menu::handleInput();
    }
}
