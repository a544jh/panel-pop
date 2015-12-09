/*
 * Game.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef GAME_H_
#define GAME_H_

#include "Board.h"
#include "KeyboardController.h"

class Game {
public:
	Game();
	Board _board;
	virtual ~Game();
	void tick();
	bool isPaused();
private:
	KeyboardController _kbc;
	bool _paused;
};

#endif /* GAME_H_ */
