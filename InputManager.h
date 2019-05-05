/*
 * InputManager.h
 *
 *  Created on: Aug 19, 2015
 *      Author: axel
 */

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <vector>
#include <SDL_joystick.h>
#include <SDL_events.h>
#include "InputEvents/InputEvent.h"

class InputManager {
 public:
  static InputManager &getInstance();

  void poll();
  void detectJoysticks();
  SDL_Joystick *getJoystick(SDL_JoystickID id);

  bool _quit;
  int _keys_len;
  const uint8_t *_keys;
  uint8_t *_prevKeys;

  bool keyDown(int); // key pressed down this poll
  bool anyKeyDown();

  bool keyUp(int);

  bool keyPressed(int); // key is down now
  int getKeyDown();
  InputEvent * getInputConfigEvent() const;

 private:
  InputManager();

  InputManager(InputManager const &) = delete;

  void operator=(InputManager const &) = delete;

  SDL_Event _inputConfigEvent; // stores the last input event on this frame that is relevant for input config
  std::vector<SDL_Joystick *> _joysticks;

  void filterInputConfigEvent(const SDL_Event &e);
};

#endif /* INPUTMANAGER_H_ */
