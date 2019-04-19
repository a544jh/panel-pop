/*
 * OptionsMenu.h
 *
 *  Created on: 22.2.2016
 *      Author: axelw
 */

#ifndef MENUS_OPTIONSMENU_H_
#define MENUS_OPTIONSMENU_H_

#include "Menu.h"

class OptionsMenuState;

class OptionsMenu : public Menu {
 public:
  OptionsMenu(OptionsMenuState &_state);
  virtual ~OptionsMenu();
  void render() const;
 private:
  OptionsMenuState &_state;
};

#endif /* MENUS_OPTIONSMENU_H_ */
