/*
 * Menu.cpp
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#include "Menu.h"

#include <SDL2/SDL_timer.h>

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

    InputState state1 = InputState::getCurrentState(StateManager::getInstance().getKeys(1));
    InputState state2 = InputState::getCurrentState(StateManager::getInstance().getKeys(2));

    if (state1._direction == DOWN || state2._direction == DOWN) {
        inputDown();
    }
    if (state1._direction == UP || state2._direction == UP) {
        inputUp();
    }
    if (state1._direction == LEFT || state2._direction == LEFT) {
        inputLeft();
    }
    if (state1._direction == RIGHT || state2._direction == RIGHT) {
        inputRight();
    }
    if (state1._swap || state2._swap) {
        inputEnter();
    }
    if (state1._raiseStack || state2._raiseStack) {
        inputCancel();
    }
}
