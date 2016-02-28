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

#include "../Config/KeyConfig.h"
class KeyboardController: public BoardController {
public:
	KeyboardController(Board&, const KeyConfig&);
	void tick();
	virtual ~KeyboardController();
	const KeyConfig _config;
	const uint32_t REPEAT_MS = 200; //until direction starts repeating

private:
	Direction _directionHeld;
	uint32_t _holdBegin;
	int getDirectionKey(Direction);
};

#endif /* GAME_KEYBOARDCONTROLLER_H_ */
