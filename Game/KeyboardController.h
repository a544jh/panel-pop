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
#include <array>

class KeyboardController: public BoardController {
public:
	KeyboardController(Board&);
	void tick();
	virtual ~KeyboardController();
};

#endif /* GAME_KEYBOARDCONTROLLER_H_ */
