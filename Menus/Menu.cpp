/*
 * Menu.cpp
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#include "Menu.h"

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
	if (_selection < 0) {
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
	if (_items.at(_selection).isChangable()) {
		_items.at(_selection).decrease();
	}
}

void Menu::inputRight() {
	if (_items.at(_selection).isChangable()) {
		_items.at(_selection).increase();
	}
}

void Menu::inputEnter() {
	_items.at(_selection).getFn()();
}

void Menu::inputCancel() {
}

void Menu::resetCursor() {
	_selection = 0;
}
