/*
 * Game.h
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#ifndef GAME_H_
#define GAME_H_

#include <cstdint>

#include "../Menus/PauseMenu.h"
#include "Board.h"

class GameEventHandler;


class Game {

public:

	enum class State {
		RUNNING, PAUSED, ENDED
	};
	Game(GameEventHandler*, BoardEventHandler*, BoardEventHandler*);

	GameEventHandler* _eventHandler;
	BoardEventHandler* _beh1;
	BoardEventHandler* _beh2;

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
	State getState() const;
	PauseMenu& getPauseMenu();

private:
	State _state;
	int _ticksRun;
	bool _advanceTick; //debug "frame advance" feature
	uint32_t _startTime;
	uint32_t _pausedTime;
	int _p1MatchPoints;
	int _p2MatchPoints;
	int _p1Points;
	int _p2Points;

	PauseMenu _pauseMenu;

	void handleGarbageSpawning(Board&, Board&);
	void handleEnd();
};

#endif /* GAME_H_ */
