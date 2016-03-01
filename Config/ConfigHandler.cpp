/*
 * ConfigHandler.cpp
 *
 *  Created on: 11.2.2016
 *      Author: axelw
 */

#include "ConfigHandler.h"
#include "../SDLContext.h"
#include "../States/StateManager.h"

#include <boost/property_tree/detail/ptree_implementation.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mixer.h>
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

KeyConfig ConfigHandler::getKeyConfig(int player) {
	KeyConfig conf;
	std::ostringstream confKey;
	std::string name;

	try {

#define X(key) confKey.clear();\
	confKey.str("");\
	confKey << "keys.p" << player << "_" << #key;\
	name = _settingsTree.get<std::string>(confKey.str());\
	conf.key = SDL_GetScancodeFromName(name.c_str());

		KEYS

#undef X

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "not all keys defined for player " << player
				<< ", using defaults\n";
		return DEFAULT_KEYS;
	}

	return conf;
}

void ConfigHandler::setKeyConfig(KeyConfig config, int player) {
	std::ostringstream confKey;

#define X(key) confKey.clear();\
		confKey.str("");\
		confKey << "keys.p" << player << "_" << #key;\
		_settingsTree.put(confKey.str(), SDL_GetScancodeName((SDL_Scancode) config.key));

	KEYS

#undef X

	StateManager::getInstance().setKeys(config, player);
}

void ConfigHandler::setFullscreen(bool fs) {
	_settingsTree.put("video.fullscreen", fs);
	if (fs != SDLContext::getInstance().isFullscreen()) {
		SDLContext::getInstance().toggleFullscreen();
	}
}

void ConfigHandler::setMusicVolume(int vol) {
	_settingsTree.put("audio.music_volume", vol);
	Mix_VolumeMusic(vol);
}

void ConfigHandler::setSfxVolume(int vol) {
	_settingsTree.put("audio.sfx_volume", vol);
	Mix_Volume(-1, vol);
}

bool ConfigHandler::getFullscreen() {
	return _settingsTree.get("video.fullscreen", false);
}

int ConfigHandler::getMusicVolume() {
	return _settingsTree.get("audio.music_volume", MIX_MAX_VOLUME);
}

int ConfigHandler::getSfxVolume() {
	return _settingsTree.get("audio.sfx_volume", MIX_MAX_VOLUME);
}

int ConfigHandler::getEndlessHighScore() {
	return _settingsTree.get("endless.high_score", 0);
}

void ConfigHandler::setEndlessHighScore(int score) {
	_settingsTree.put("endless.high_score", score);
}
