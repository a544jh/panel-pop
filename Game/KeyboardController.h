/*
 * KeyboardController.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef GAME_KEYBOARDCONTROLLER_H_
#define GAME_KEYBOARDCONTROLLER_H_

#include "BoardController.h"
#include "../InputManager.h"

#include "../InputState.h"

class KeyboardController : public BoardController {
 public:
  KeyboardController(Board &, InputConfig &);
  void tick();
  virtual ~KeyboardController();
  InputConfig &_config;
  const uint32_t REPEAT_MS = 200; //until direction starts repeating

 private:
  Direction _directionHeld;
  uint32_t _holdBegin;
};

#endif /* GAME_KEYBOARDCONTROLLER_H_ */
