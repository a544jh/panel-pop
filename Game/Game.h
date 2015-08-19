/*
 * Game.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef GAME_H_
#define GAME_H_

#include "Board.h"
#include "../InputManager.h"
#include <array>

class Game {
public:
	Game();
	Board _board;
	virtual ~Game();
	void tick();
private:
	std::array<bool, KEYBOARD_SIZE> _prevInput;
};

#endif /* GAME_H_ */
