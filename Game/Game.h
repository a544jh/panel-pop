/*
 * Game.h
 *
 *  Created on: 24.2.2016
 *      Author: axelw
 */

#ifndef GAME_GAME_H_
#define GAME_GAME_H_

#include <stdint.h>
#include <memory>

#include "../Menus/PauseMenu.h"

class Board;

class GameEventHandler;

class Game {
 public:

  enum class State {
    RUNNING, PAUSED, ENDED
  };

  Game();
  virtual ~Game();

  virtual Board &getBoard(int id) = 0;

  virtual void tick() = 0;
  virtual void reset() = 0;
  void inputTogglePause();
  bool isPaused() const;
  int getTicksRun() const;
  uint32_t getTime(); //for how many milliseconds the Game has been run
  State getState() const;
  PauseMenu &getPauseMenu();
  bool isPanic() const;
  uint32_t getPausedTime() const;
  uint32_t getStartTime() const;
  static const int COUNTDOWN_STEP_MS = 1000;

 protected:
  std::unique_ptr<GameEventHandler> _eventHandler;
  State _state;
  int _ticksRun;
  bool _panic;
  uint32_t _startTime;
  uint32_t _pausedTime;
  PauseMenu _pauseMenu;
  int _lastContdownMS;

  virtual void handleEnd() = 0;
};

#endif /* GAME_GAME_H_ */
