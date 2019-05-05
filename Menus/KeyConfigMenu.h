/*
 * KeyConfigMenu.h
 *
 *  Created on: 14.2.2016
 *      Author: axelw
 */

#ifndef MENUS_KEYCONFIGMENU_H_
#define MENUS_KEYCONFIGMENU_H_

#include "Menu.h"
#include "../Config/InputConfig.h"

class OptionsMenuState;

class KeyConfigMenu : public Menu {
 public:
  KeyConfigMenu(OptionsMenuState &, int player);
  virtual ~KeyConfigMenu();

  void render() const;

  void handleInput();

 private:
  OptionsMenuState &_state;
  int _player;
  std::shared_ptr<InputEvent> *_inputToSet;
  bool _waitingForKey;

  InputConfig _newKeyConfig;
  void setActiveInput(InputEvent *event);
};

#endif /* MENUS_KEYCONFIGMENU_H_ */
