/*
 * ConfigHandler.cpp
 *
 *  Created on: 11.2.2016
 *      Author: axelw
 */

#include "ConfigHandler.h"
#include "../SDLContext.h"
#include "../States/StateManager.h"
#include "../InputEvents/JoyHat.h"
#include "../InputEvents/JoyButton.h"

#include <boost/property_tree/detail/ptree_implementation.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mixer.h>
#include <exception>
#include <iostream>
#include <string>

InputConfig ConfigHandler::DEFAULT_KEYS = InputConfig(new KeyboardKey(SDL_SCANCODE_UP),
                                                      new KeyboardKey(SDL_SCANCODE_DOWN),
                                                      new KeyboardKey(SDL_SCANCODE_LEFT),
                                                      new KeyboardKey(SDL_SCANCODE_RIGHT),
                                                      new KeyboardKey(SDL_SCANCODE_X),
                                                      new KeyboardKey(SDL_SCANCODE_Z)
);

InputConfig ConfigHandler::DEFAULT_JOYSTICK = InputConfig(new JoyHat(0,0,SDL_HAT_UP),
                                                      new JoyHat(0,0,SDL_HAT_DOWN),
                                                      new JoyHat(0,0,SDL_HAT_LEFT),
                                                      new JoyHat(0,0,SDL_HAT_RIGHT),
                                                      new JoyButton(0, 0),
                                                      new JoyButton(0,1)
);

ConfigHandler::ConfigHandler() {
}

ConfigHandler &ConfigHandler::getInstance() {
    static ConfigHandler instance;
    return instance;
}

bool ConfigHandler::loadConfig() {
    try {
        boost::property_tree::read_ini(CONFIG_FILENAME, _settingsTree);
    } catch (std::exception &e) {
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

InputConfig ConfigHandler::getKeyConfig(int player) {
    // TODO: implement
    if (player == 1) {
        return DEFAULT_KEYS;
    } else {
        return DEFAULT_JOYSTICK;
    }
}

void ConfigHandler::setKeyConfig(InputConfig config, int player) {

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
