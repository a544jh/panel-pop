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
	Board _board2;
	virtual ~Game();
	void tick();
	void reset();
	void inputTogglePause();
	void inputAdvanceTick();//debug "frame advance" feature
	const bool isPaused() const;

	int getTicksRun() const {
		return _ticksRun;
	}

	bool isAdvanceTick() const;
	uint32_t getTime();

private:
	bool _paused;
	int _ticksRun;
	void handleGarbageSpawning(Board&, Board&);
	bool _advanceTick; //debug "frame advance" feature
	uint32_t _startTime;
	uint32_t _pausedTime;
};

#endif /* GAME_H_ */
