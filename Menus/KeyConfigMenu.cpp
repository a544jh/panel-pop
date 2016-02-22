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
#include "../States/StateManager.h"
#include "MenuItem.h"

KeyConfigMenu::KeyConfigMenu(OptionsMenuState& state, int player) :
				_state(state),
				_player(player),
				_activeKey(nullptr),
				_waitingForKey(false) {

	_newKeyConfig = ConfigHandler::getInstance().getKeyConfig(1);

	addItem(
			MenuItem("Up",
					[&]() {_activeKey = &_newKeyConfig.up; _waitingForKey = true;},
					_newKeyConfig.up, 0));
	addItem(
			MenuItem("Down",
					[&]() {_activeKey = &_newKeyConfig.down; _waitingForKey = true;},
					_newKeyConfig.down, 0));
	addItem(
			MenuItem("Left",
					[&]() {_activeKey = &_newKeyConfig.left; _waitingForKey = true;},
					_newKeyConfig.left, 0));
	addItem(
			MenuItem("Right",
					[&]() {_activeKey = &_newKeyConfig.right; _waitingForKey = true;},
					_newKeyConfig.right, 0));
	addItem(
			MenuItem("Swap/Accept",
					[&]() {_activeKey = &_newKeyConfig.swap; _waitingForKey = true;},
					_newKeyConfig.swap, 0));
	addItem(
			MenuItem("Raise stack/Cancel",
					[&]() {_activeKey = &_newKeyConfig.raiseStack; _waitingForKey = true;},
					_newKeyConfig.raiseStack, 0));

	addItem(MenuItem("Apply", [&]() {
		ConfigHandler::getInstance().setKeyConfig(_newKeyConfig,_player);
		ConfigHandler::getInstance().saveConfig();
	}));

}

KeyConfigMenu::~KeyConfigMenu() {
}

void KeyConfigMenu::setActiveKey(int scancode) {
	*_activeKey = scancode;
	_items.at(_selection).setValue(scancode);
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
		_SDLContext.renderText(text, { 0, 0, 0 }, _SDLContext._fontPs, x, y);

		const char* name = SDL_GetScancodeName(
				(SDL_Scancode) _items.at(i).getValue());

		_SDLContext.renderText(name, { 0, 0, 0 }, _SDLContext._fontPs, x + 350,
				y);
	}

	if (_waitingForKey) {
		_SDLContext.renderText("--press key--", { 0, 0, 0 },
				_SDLContext._fontPs, 0, 300);
	}

}

void KeyConfigMenu::handleInput() {
	InputManager& input = InputManager::getInstance();
	if (_waitingForKey) {
		int pressed = input.getKeyDown();
		if (pressed != 0) {
			setActiveKey(pressed);
			_waitingForKey = false;
		}
	} else {
		Menu::handleInput();
	}
}
