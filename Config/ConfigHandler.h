/*
 * ConfigHandler.h
 *
 *  Created on: 11.2.2016
 *      Author: axelw
*/

#ifndef CONFIG_CONFIGHANDLER_H_
#define CONFIG_CONFIGHANDLER_H_

#include <boost/property_tree/ptree.hpp>
#include <SDL2/SDL_scancode.h>
#include "InputConfig.h"
#include "../InputEvents/KeyboardKey.h"

class ConfigHandler {
 public:

  static ConfigHandler &getInstance();

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

  boost::property_tree::ptree _settingsTree;

  // Populate the tree with the built-in bindings (see DEFAULT_BINDINGS).
  void applyDefaultKeyConfig();
  // Binding for one action, falling back to the built-in default when the
  // config is missing the key or holds an unparseable value. Never null.
  InputEvent *parseInputEvent(int player, const std::string &action);
  // Parse a serialized binding ("KUp", "J0_B1", ...); null if unparseable.
  static InputEvent *parseBinding(const std::string &value);
};

#endif /* CONFIG_CONFIGHANDLER_H_ */
