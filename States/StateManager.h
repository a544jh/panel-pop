/*
 * StateManager.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#include <cstdint>

#include "../Config/KeyboardControllerConfig.h"

class InputManager;
class SDLContext;
class State;

class StateManager {
public:
	static StateManager& getInstance();
	void run();
	void switchToState(State*);
	void startGame();
	void returnToTile();
	float getAvgFps() const;
	const KeyboardControllerConfig& getP1keys() const;
	void setP1keys(const KeyboardControllerConfig& p1keys);
	const KeyboardControllerConfig& getP2keys() const;
	void setP2keys(const KeyboardControllerConfig& p2keys);
	void goBack();

private:
	StateManager();
	StateManager(StateManager const&) = delete;
	void operator=(StateManager const&) = delete;
	State* _currentState;
	State* _newState;
	bool _switchState;
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
	KeyboardControllerConfig _p1keys, _p2keys;
};

#endif /* STATEMANAGER_H_ */
