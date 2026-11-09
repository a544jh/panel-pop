/*
 * StateManager.h
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#include <cstdint>
#include <array>

#include "../Config/InputConfig.h"

class InputManager;
class SDLContext;
class State;

class StateManager {
 public:
  static StateManager &getInstance();
  void run();
  // Advance exactly one logic tick (and optionally render it) without waiting
  // for wall-clock pacing. Used by the headless debug console.
  bool step(bool render);
  bool running() const;
  int logicTicks() const;
  void switchToState(State *);
  void returnToTitle();
  float getAvgFps() const;
  State *getCurrentState() const;
  InputConfig &getKeys(int);
  void setKeys(InputConfig, int);
  void goBack();
  void quit();

 private:
  StateManager();
  StateManager(StateManager const &) = delete;
  void operator=(StateManager const &) = delete;
  bool stepLogic();
  void stepRender();
  void applyPendingStateSwitch();
  State *_currentState;
  State *_newState;
  bool _switchState;
  SDLContext &SDL;
  InputManager &input;
  uint32_t _lastTick;
  uint32_t _frameTime;
  int _logicTicks;
  bool _running;
  uint32_t _framesRun;
  uint32_t _startTime;
  uint32_t _lastFrame;
  float _avgFps;
  bool _showFps;
  void showFps();
  std::array<InputConfig, 2> _inputConfigs;
};

#endif /* STATEMANAGER_H_ */
