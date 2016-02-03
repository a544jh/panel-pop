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

	static const int MATCH_POINTS = 2;

	virtual ~Game();
	void tick();
	void reset();
	void inputTogglePause();
	void inputAdvanceTick(); //debug "frame advance" feature
	const bool isPaused() const;

	int getTicksRun() const {
		return _ticksRun;
	}

	bool isAdvanceTick() const;
	uint32_t getTime();
	int getP1MatchPoints() const;
	int getP1Points() const;
	int getP2MatchPoints() const;
	int getP2Points() const;

private:
	enum class State {
		RUNNING, PAUSED, ENDED
	} _state;
	int _ticksRun;
	bool _advanceTick; //debug "frame advance" feature
	uint32_t _startTime;
	uint32_t _pausedTime;
	int _p1MatchPoints;
	int _p2MatchPoints;
	int _p1Points;
	int _p2Points;

	void handleGarbageSpawning(Board&, Board&);
};

#endif /* GAME_H_ */
