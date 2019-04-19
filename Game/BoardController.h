/*
 * BoardController.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef GAME_BOARDCONTROLLER_H_
#define GAME_BOARDCONTROLLER_H_
#include "Board.h"

class BoardController {
 public:
  BoardController(Board &);
  virtual void tick() = 0;
  virtual ~BoardController();
 protected:
  Board &_board;
};

#endif /* GAME_BOARDCONTROLLER_H_ */
