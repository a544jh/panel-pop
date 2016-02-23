/*
 * ConfigHandler.h
 *
 *  Created on: 11.2.2016
 *      Author: axelw
 */

#ifndef CONFIG_CONFIGHANDLER_H_
#define CONFIG_CONFIGHANDLER_H_

#include <boost/property_tree/ptree.hpp>
#include "KeyboardControllerConfig.h"
#include <SDL2/SDL_scancode.h>

class ConfigHandler {
public:

	static ConfigHandler& getInstance();

	static constexpr const char* CONFIG_FILENAME = "panelpop.ini";
	static constexpr const KeyboardControllerConfig DEFAULT_KEYS = {
			SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT,
			SDL_SCANCODE_RIGHT, SDL_SCANCODE_X, SDL_SCANCODE_Z };

	bool loadConfig();
	bool saveConfig();

	KeyboardControllerConfig getKeyConfig(int player);
	void setKeyConfig(KeyboardControllerConfig, int player);
	void setFullscreen(bool);
	void setMusicVolume(int);
	void setSfxVolume(int);

	bool getFullscreen();
	int getMusicVolume();
	int getSfxVolume();

private:
	ConfigHandler();
	ConfigHandler(ConfigHandler const&) = delete;
	void operator=(ConfigHandler const&) = delete;

	boost::property_tree::ptree _settingsTree;
};

#endif /* CONFIG_CONFIGHANDLER_H_ */
