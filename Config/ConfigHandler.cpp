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
#include <iostream>

ConfigHandler::ConfigHandler() {
}

ConfigHandler& ConfigHandler::getInstance() {
	static ConfigHandler instance;
	return instance;
}

bool ConfigHandler::loadConfig() {
	std::cout << "trying to load...";
	try {
		boost::property_tree::read_ini(CONFIG_FILENAME, _settingsTree);
	} catch (...) {
		std::cout << "error";
	}
	return false;
}

bool ConfigHandler::saveConfig() {
	const char* string = SDL_GetScancodeName(SDL_SCANCODE_UP);
	_settingsTree.put("keys.p1_up", string);
	boost::property_tree::write_ini(CONFIG_FILENAME, _settingsTree);
	return true;
}
