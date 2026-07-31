/*
 * ConfigHandler.cpp
 *
 *  Created on: 11.2.2016
 *      Author: axelw
 */

#include "ConfigHandler.h"
#include "../SDLContext.h"
#include "../Util/Paths.h"
#include "../States/StateManager.h"
#include "../InputEvents/JoyHat.h"
#include "../InputEvents/JoyButton.h"
#include "../InputEvents/JoyAxisDirection.h"

#include <boost/property_tree/detail/ptree_implementation.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mixer.h>
#include <exception>
#include <iostream>
#include <string>

namespace {

struct KeyBindingDefault {
    const char *action;
    const char *p1;
    const char *p2;
};

// Bindings used when the config file is absent or incomplete: P1 on the
// keyboard, P2 on the first gamepad. Without these a fresh install has no key
// config at all, since the config file is only created on first run.
const KeyBindingDefault DEFAULT_BINDINGS[] = {
    {"up", "KUp", "J0_A1_-"},
    {"down", "KDown", "J0_A1_+"},
    {"left", "KLeft", "J0_A0_-"},
    {"right", "KRight", "J0_A0_+"},
    {"swap", "KX", "J0_B1"},
    {"raiseStack", "KZ", "J0_B2"},
    {"start", "KReturn", "J0_B9"},
};

std::string defaultBinding(int player, const std::string &action) {
    for (const KeyBindingDefault &binding : DEFAULT_BINDINGS) {
        if (action == binding.action) {
            return player == 2 ? binding.p2 : binding.p1;
        }
    }
    return "";
}

} // namespace

ConfigHandler::ConfigHandler() {
}

ConfigHandler &ConfigHandler::getInstance() {
    static ConfigHandler instance;
    return instance;
}

bool ConfigHandler::loadConfig() {
    try {
        boost::property_tree::read_ini(Paths::configFile(), _settingsTree);
    } catch (std::exception &e) {
        std::cerr << "error in reading config file, using defaults..."
                  << std::endl;
        std::cerr << e.what() << std::endl;
        // Seed the defaults and write them out, so a fresh install both starts
        // up and leaves the user a config file to edit.
        applyDefaultKeyConfig();
        saveConfig();
        return false;
    }
    return true;
}

bool ConfigHandler::saveConfig() {
    try {
        boost::property_tree::write_ini(Paths::configFile(), _settingsTree);
    } catch (std::exception &e) {
        std::cerr << "error writing config file: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void ConfigHandler::applyDefaultKeyConfig() {
    for (const KeyBindingDefault &binding : DEFAULT_BINDINGS) {
        _settingsTree.put(std::string("keys.p1_") + binding.action, binding.p1);
        _settingsTree.put(std::string("keys.p2_") + binding.action, binding.p2);
    }
}

InputConfig ConfigHandler::getKeyConfig(int player) {

    return InputConfig(parseInputEvent(player, "up"),
                       parseInputEvent(player, "down"),
                       parseInputEvent(player, "left"),
                       parseInputEvent(player, "right"),
                       parseInputEvent(player, "swap"),
                       parseInputEvent(player, "raiseStack"),
                       parseInputEvent(player, "start"));

}

InputEvent *ConfigHandler::parseInputEvent(int player, const std::string &action) {
    auto configKey = "keys.p" + std::to_string(player) + "_" + action;
    auto fallback = defaultBinding(player, action);

    InputEvent *event = parseBinding(_settingsTree.get<std::string>(configKey, fallback));
    if (event == nullptr) {
        // Configured value was unparseable; fall back to the built-in binding.
        event = parseBinding(fallback);
    }
    if (event == nullptr) {
        // Nothing usable at all: an unbound key that simply never fires.
        event = new KeyboardKey(SDL_SCANCODE_UNKNOWN);
    }
    return event;
}

InputEvent *ConfigHandler::parseBinding(const std::string &value) {
    if (value.empty()) {
        return nullptr;
    }

    try {
        switch (value[0]) {
            case 'K':return new KeyboardKey(SDL_GetScancodeFromName(value.substr(1).c_str()));
            case 'J': {
                auto jidEnd = value.find('_', 1);
                if (jidEnd == std::string::npos) {
                    break;
                }
                int joystickId = std::stoi(value.substr(1, jidEnd - 1));
                switch (value[jidEnd + 1]) {
                    case 'B': {
                        int buttonId = std::stoi(value.substr(jidEnd + 2));
                        return new JoyButton(joystickId, buttonId);
                    }
                    case 'H': {
                        auto hidEnd = value.find('_', jidEnd + 1);
                        if (hidEnd == std::string::npos) {
                            break;
                        }
                        int hatId = std::stoi(value.substr(jidEnd + 2, hidEnd - jidEnd - 2));
                        int hatDir = std::stoi(value.substr(hidEnd + 1));
                        return new JoyHat(joystickId, hatId, hatDir);
                    }
                    case 'A': {
                        auto aidEnd = value.find('_', jidEnd + 1);
                        if (aidEnd == std::string::npos) {
                            break;
                        }
                        int axisId = std::stoi(value.substr(jidEnd + 2, aidEnd - jidEnd - 2));
                        JoyAxisDirection::Direction
                            axisDir = value[aidEnd + 1] == '+' ? JoyAxisDirection::POSITIVE : JoyAxisDirection::NEGATIVE;
                        return new JoyAxisDirection(joystickId, axisId, axisDir);
                    }
                }
            }
        }
    } catch (std::exception &) {
        // Malformed numeric field; handled as an unparseable binding.
    }

    return nullptr;
}

void ConfigHandler::setKeyConfig(InputConfig config, int player) {

    StateManager::getInstance().setKeys(config, player);
    auto prefix = "keys.p" + std::to_string(player) + "_";
    _settingsTree.put(prefix + "up", config._up->toString());
    _settingsTree.put(prefix + "down", config._down->toString());
    _settingsTree.put(prefix + "left", config._left->toString());
    _settingsTree.put(prefix + "right", config._right->toString());
    _settingsTree.put(prefix + "swap", config._swap->toString());
    _settingsTree.put(prefix + "raiseStack", config._raiseStack->toString());
    _settingsTree.put(prefix + "start", config._start->toString());

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
