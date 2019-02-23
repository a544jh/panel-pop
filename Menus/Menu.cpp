/*
 * Menu.cpp
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#include "Menu.h"

#include <SDL2/SDL_timer.h>

#include "../Config/KeyConfig.h"
#include "../InputManager.h"
#include "../States/StateManager.h"

Menu::Menu() :
_selection(0),
_time(SDL_GetTicks()) {
}

Menu::~Menu() {
}

void Menu::addItem(MenuItem menuItem) {
    _items.push_back(menuItem);
}

void Menu::inputUp() {
    --_selection;
    if (_selection > _items.size()) {
        _selection = _items.size() - 1;
    }
    _time = SDL_GetTicks();
}

void Menu::inputDown() {
    ++_selection;
    if (_selection >= _items.size()) {
        _selection = 0;
    }
    _time = SDL_GetTicks();
}

void Menu::inputLeft() {
    if (_items.at(_selection).getOptionType() != MenuItem::OptionType::NONE) {
        _items.at(_selection).decrease();
    }
}

void Menu::inputRight() {
    if (_items.at(_selection).getOptionType() != MenuItem::OptionType::NONE) {
        _items.at(_selection).increase();
    }
}

void Menu::inputEnter() {
    _items.at(_selection).getFn()();
    _time = SDL_GetTicks();
}

void Menu::inputCancel() {
    StateManager::getInstance().goBack();
}

void Menu::resetCursor() {
    _selection = 0;
}

void Menu::handleInput() {
    InputManager& input = InputManager::getInstance();
    KeyConfig p1keys = StateManager::getInstance().getP1keys();
    KeyConfig p2keys = StateManager::getInstance().getP2keys();
    if (input.keyDown(p1keys.down) || input.keyDown(p2keys.down)) {
        inputDown();
    }
    if (input.keyDown(p1keys.up) || input.keyDown(p2keys.up)) {
        inputUp();
    }
    if (input.keyDown(p1keys.left) || input.keyDown(p2keys.left)) {
        inputLeft();
    }
    if (input.keyDown(p1keys.right) || input.keyDown(p2keys.right)) {
        inputRight();
    }
    if (input.keyDown(p1keys.swap) || input.keyDown(p2keys.swap)) {
        inputEnter();
    }
    if (input.keyDown(p1keys.raiseStack)
            || input.keyDown(p2keys.raiseStack)) {
        inputCancel();
    }
}
