/*
 * MainMenu.h
 *
 *  Created on: 22.2.2016
 *      Author: axelw
 */

#ifndef MENUS_MAINMENU_H_
#define MENUS_MAINMENU_H_

#include "Menu.h"

class MainMenu : public Menu {
 public:
  MainMenu();
  virtual ~MainMenu();
  void render() const;
};

#endif /* MENUS_MAINMENU_H_ */
