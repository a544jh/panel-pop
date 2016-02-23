/*
 * OptionsMenu.cpp
 *
 *  Created on: 22.2.2016
 *      Author: axelw
 */

#include "OptionsMenu.h"

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_timer.h>
#include <string>
#include <vector>

#include "../Config/ConfigHandler.h"
#include "../SDLContext.h"
#include "MenuItem.h"

OptionsMenu::OptionsMenu() {

	ConfigHandler& cf = ConfigHandler::getInstance();

	addItem(
			MenuItem("Fullscreen", [&]() {}, cf.getFullscreen(), 1,
					MenuItem::OptionType::TOGGLE));
	addItem(
			MenuItem("Music volume", [&]() {}, cf.getMusicVolume(), MIX_MAX_VOLUME,
					MenuItem::OptionType::SLIDER));
	addItem(
			MenuItem("SFX volume", [&]() {}, cf.getSfxVolume(), MIX_MAX_VOLUME,
					MenuItem::OptionType::SLIDER));
	addItem(
			MenuItem("Configure keys", [&]() {}, 0, 1,
					MenuItem::OptionType::PLAYER));
	addItem(MenuItem("Apply", [&](){
		cf.setFullscreen(_items.at(0).getValue());
		cf.setMusicVolume(_items.at(1).getValue());
		cf.setSfxVolume(_items.at(2).getValue());
		cf.saveConfig();
		inputCancel();
	}));
}

OptionsMenu::~OptionsMenu() {
}

void OptionsMenu::render() const {
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

		std::string option = _items.at(i).getOptionString();

		_SDLContext.renderText(option, { 0, 0, 0 }, _SDLContext._fontPs, x + 350,
				y);
	}
}
