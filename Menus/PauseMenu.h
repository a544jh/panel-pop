/*
 * PauseMenu.h
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#ifndef MENUS_PAUSEMENU_H_
#define MENUS_PAUSEMENU_H_

#include "Menu.h"

class Game;

class PauseMenu : public Menu {
 public:
  PauseMenu(Game &);
  virtual ~PauseMenu();
  void render() const;
  void inputCancel();

 private:
  Game &_game;
};

#endif /* MENUS_PAUSEMENU_H_ */
