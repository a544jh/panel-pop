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
#include "../InputEvents/JoyAxisDirection.h"

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mixer.h>
#include <exception>
#include <iostream>
#include <string>
#include <json/json.h>
#include <fstream>

#ifdef __vita__
    #include <psp2/kernel/iofilemgr.h>
#endif

ConfigHandler::ConfigHandler() {
}

ConfigHandler &ConfigHandler::getInstance() {
    static ConfigHandler instance;
    return instance;
}

bool ConfigHandler::loadConfig() {
    std::ifstream configFile;
    std::string configPath = CONFIG_DIR + CONFIG_FILENAME;
    configFile.open(configPath, std::ios::in);
    #ifdef __vita__
        if (!configFile) {
            sceIoMkdir(CONFIG_DIR.c_str(), 0777);
            configPath = "app0:/" + CONFIG_FILENAME;
            configFile.open(configPath, std::ios::in);
        }
    #endif
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, configFile, &_settingsTree, &errs)) {
        std::cerr << "error in reading config file, using defaults..." << std::endl;
        std::cerr << errs;
        configFile.close();
        return false;
    }
    configFile.close();
    return true;
}

bool ConfigHandler::saveConfig() {
    std::string configPath = CONFIG_DIR + CONFIG_FILENAME;
    std::ofstream configFile(configPath, std::ios::binary);
    configFile << _settingsTree;
    configFile.close();
    return true;
}

InputConfig ConfigHandler::getKeyConfig(int player) {

    auto prefix = "p" + std::to_string(player) + "_";
    return InputConfig(parseInputEvent(prefix + "up"),
                       parseInputEvent(prefix + "down"),
                       parseInputEvent(prefix + "left"),
                       parseInputEvent(prefix + "right"),
                       parseInputEvent(prefix + "swap"),
                       parseInputEvent(prefix + "raiseStack"),
                       parseInputEvent(prefix + "start"));

}

InputEvent *ConfigHandler::parseInputEvent(const std::string &configKey) {
    std::string value = _settingsTree["keys"].get(configKey, "").asString();

    char type = value[0];
    switch (type) {
        case 'K':return new KeyboardKey(SDL_GetScancodeFromName(value.substr(1).c_str()));
        case 'J': {
            auto jidEnd = value.find('_', 1);
            auto jidStr = value.substr(1, jidEnd - 1);
            int joystickId = std::stoi(jidStr);
            char eventType = value[jidEnd + 1];
            switch (eventType) {
                case 'B': {
                    int buttonId = std::stoi(value.substr(jidEnd + 2));
                    return new JoyButton(joystickId, buttonId);
                }
                case 'H': {
                    int hidEnd = value.find('_', jidEnd + 1);
                    int hatId = std::stoi(value.substr(jidEnd + 2, (hidEnd - 1 - (jidEnd + 1))));
                    int hatDir = std::stoi(value.substr(hidEnd + 1));
                    return new JoyHat(joystickId, hatId, hatDir);
                }
                case 'A': {
                    int aidEnd = value.find('_', jidEnd + 1);
                    int axisId = std::stoi(value.substr(jidEnd + 2, (aidEnd - 1 - (jidEnd + 1))));
                    JoyAxisDirection::Direction
                        axisDir = value[aidEnd + 1] == '+' ? JoyAxisDirection::POSITIVE : JoyAxisDirection::NEGATIVE;
                    return new JoyAxisDirection(joystickId, axisId, axisDir);
                }
            }
        }
    } // TODO: handle errors lol
}

void ConfigHandler::setKeyConfig(InputConfig config, int player) {
    StateManager::getInstance().setKeys(config, player);
    auto prefix = "p" + std::to_string(player) + "_";
    _settingsTree["keys"][prefix + "up"] = config._up->toString();
    _settingsTree["keys"][prefix + "down"] = config._down->toString();
    _settingsTree["keys"][prefix + "left"] = config._left->toString();
    _settingsTree["keys"][prefix + "right"] = config._right->toString();
    _settingsTree["keys"][prefix + "swap"] = config._swap->toString();
    _settingsTree["keys"][prefix + "raiseStack"] = config._raiseStack->toString();
    _settingsTree["keys"][prefix + "start"] = config._start->toString();

}

void ConfigHandler::setFullscreen(bool fs) {
    _settingsTree["video"]["fullscreen"] = fs;
    if (fs != SDLContext::getInstance().isFullscreen()) {
        SDLContext::getInstance().toggleFullscreen();
    }
}

void ConfigHandler::setMusicVolume(int vol) {
    _settingsTree["audio"]["music_volume"] = vol;
    Mix_VolumeMusic(vol);
}

void ConfigHandler::setSfxVolume(int vol) {
    _settingsTree["audio"]["sfx_volume"] = vol;
    Mix_Volume(-1, vol);
}

bool ConfigHandler::getFullscreen() {
    return _settingsTree["video"].get("fullscreen", false).asBool();
}

int ConfigHandler::getMusicVolume() {
    return _settingsTree["audio"].get("music_volume", MIX_MAX_VOLUME).asInt();
}

int ConfigHandler::getSfxVolume() {
    return _settingsTree["audio"].get("sfx_volume", MIX_MAX_VOLUME).asInt();
}

int ConfigHandler::getEndlessHighScore() {
    return _settingsTree["endless"].get("high_score", 0).asInt();
}

void ConfigHandler::setEndlessHighScore(int score) {
    _settingsTree["endless"]["high_score"] = score;
}
