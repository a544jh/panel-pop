/*
 * StateManager.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#include "State.h"
#include "../InputManager.h"
#include "../SDLContext.h"

class StateManager {
public:
	static StateManager& getInstance();
	void run();
	void switchToState(State*);
	void startGame();
	void returnToTile();
	float getAvgFps() const;

private:
	StateManager();
	StateManager(StateManager const&) = delete;
	void operator=(StateManager const&) = delete;
	State* _currentState;
	SDLContext& SDL;
	InputManager& input;
	uint32_t _lastTick;
	uint32_t _frameTime;
	bool _running;
	uint32_t _framesRun;
	uint32_t _startTime;
	uint32_t _lastFrame;
	float _avgFps;
	bool _showFps;
	void showFps();
};

#endif /* STATEMANAGER_H_ */
