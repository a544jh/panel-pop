/*
 * ConfigHandler.cpp
 *
 *  Created on: 11.2.2016
 *      Author: axelw
 */

#include "ConfigHandler.h"

#include <boost/property_tree/detail/ptree_implementation.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <exception>
#include <iostream>
#include <string>

ConfigHandler::ConfigHandler() {
}

ConfigHandler& ConfigHandler::getInstance() {
	static ConfigHandler instance;
	return instance;
}

bool ConfigHandler::loadConfig() {
	try {
		boost::property_tree::read_ini(CONFIG_FILENAME, _settingsTree);
	} catch (std::exception& e) {
		std::cerr << "error in reading config file, using defaults..."
				<< std::endl;
		std::cerr << e.what();
		return false;
	}
	return true;
}

bool ConfigHandler::saveConfig() {
	std::string string = SDL_GetScancodeName(SDL_SCANCODE_UP);
	_settingsTree.put("keys.p1_up", string);
	boost::property_tree::write_ini(CONFIG_FILENAME, _settingsTree);
	return true;
}

KeyboardControllerConfig ConfigHandler::getKeyConfig(int player) {
	KeyboardControllerConfig conf;
	std::ostringstream key;
	key << "keys.p" << player << "_up";
	std::string name = _settingsTree.get<std::string>(key.str(),
			SDL_GetScancodeName(SDL_SCANCODE_A));
	conf.up = SDL_GetScancodeFromName(name.c_str());

	return conf;
}

void ConfigHandler::setKeyConfig(KeyboardControllerConfig config, int player) {
	std::ostringstream key;
	key << "keys.p" << player << "_up";
	_settingsTree.put(key.str(), SDL_GetScancodeName((SDL_Scancode) config.up));
}
