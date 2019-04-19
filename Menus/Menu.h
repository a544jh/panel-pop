/*
 * Menu.h
 *
 *  Created on: 6.2.2016
 *      Author: axelw
 */

#ifndef MENUS_MENU_H_
#define MENUS_MENU_H_

#include <vector>

#include "../Rendering/Renderer.h"
#include "MenuItem.h"

class Menu : public Renderer {
 public:
  Menu();
  virtual ~Menu();
  void addItem(MenuItem);
  void inputUp();
  void inputDown();
  void inputLeft();
  void inputRight();
  void inputEnter();
  virtual void inputCancel();
  void resetCursor(); //makes the first item selected again

  virtual void handleInput();

  virtual void render() const = 0;

 protected:
  std::vector<MenuItem> _items;
  unsigned _selection;
  uint32_t _time; //we can reset the time when the cursor is moved for a better visual cursor blinking experience
};

#endif /* MENUS_MENU_H_ */
