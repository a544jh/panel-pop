/*
 * ConfigHandler.h
 *
 *  Created on: 11.2.2016
 *      Author: axelw
*/

#ifndef CONFIG_CONFIGHANDLER_H_
#define CONFIG_CONFIGHANDLER_H_

#include <json/json.h>
#include <SDL2/SDL_scancode.h>
#include "InputConfig.h"
#include "../InputEvents/KeyboardKey.h"

class ConfigHandler {
 public:

  static ConfigHandler &getInstance();

#ifdef __vita__
  const char *CONFIG_FILENAME = "app0:/panelpop.json";
#else
  const char *CONFIG_FILENAME = "panelpop.json";
#endif
  bool loadConfig();
  bool saveConfig();

  InputConfig getKeyConfig(int player);
  void setKeyConfig(InputConfig, int player);
  void setFullscreen(bool);
  void setMusicVolume(int);
  void setSfxVolume(int);

  bool getFullscreen();
  int getMusicVolume();
  int getSfxVolume();

  int getEndlessHighScore();
  void setEndlessHighScore(int);

 private:
  ConfigHandler();
  ConfigHandler(ConfigHandler const &) = delete;
  void operator=(ConfigHandler const &) = delete;

  Json::Value _settingsTree;

  InputEvent *parseInputEvent(const std::string &configKey);
};

#endif /* CONFIG_CONFIGHANDLER_H_ */
