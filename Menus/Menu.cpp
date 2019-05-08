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

    // TODO: some default way of handling keys

    const InputConfig& dc = InputManager::defaultMenuConfig;

    InputConfig& config1 = StateManager::getInstance().getKeys(1);
    InputConfig& config2 = StateManager::getInstance().getKeys(2);

    if (config1.getDownDirection() == DOWN || config2.getDownDirection() == DOWN || dc.getDownDirection() == DOWN) {
        inputDown();
    }
    if (config1.getDownDirection() == UP || config2.getDownDirection() == UP || dc.getDownDirection() == UP) {
        inputUp();
    }
    if (config1.getDownDirection() == LEFT || config2.getDownDirection() == LEFT || dc.getDownDirection() == LEFT) {
        inputLeft();
    }
    if (config1.getDownDirection() == RIGHT || config2.getDownDirection() == RIGHT || dc.getDownDirection() == RIGHT) {
        inputRight();
    }
    if (config1.swapDown() || config2.swapDown() || dc.swapDown()) {
        inputEnter();
    }
    if (config1.raiseStackDown() || config2.raiseStackDown() || dc.raiseStackDown()) {
        inputCancel();
    }
}
