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
	boost::property_tree::write_ini(CONFIG_FILENAME, _settingsTree);
	return true;
}

KeyboardControllerConfig ConfigHandler::getKeyConfig(int player) {
	KeyboardControllerConfig conf;
	std::ostringstream confKey;
	std::string name;

	try{

#define X(key) confKey.clear();\
	confKey.str("");\
	confKey << "keys.p" << player << "_" << #key;\
	name = _settingsTree.get<std::string>(confKey.str());\
	conf.key = SDL_GetScancodeFromName(name.c_str());

	KEYS

#undef X

	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "not all keys defined for player " << player << ", using defaults\n";
		return DEFAULT_KEYS;
	}

	return conf;
}

void ConfigHandler::setKeyConfig(KeyboardControllerConfig config, int player) {
	std::ostringstream confKey;

#define X(key) confKey.clear();\
		confKey.str("");\
		confKey << "keys.p" << player << "_" << #key;\
		_settingsTree.put(confKey.str(), SDL_GetScancodeName((SDL_Scancode) config.key));

	KEYS

#undef X

}
