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
#include "Config/InputConfig.h"

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
  InputEvent * getInputDownEvent() const;

  static InputConfig defaultMenuConfig;

 private:
  InputManager();

  InputManager(InputManager const &) = delete;

  void operator=(InputManager const &) = delete;

  SDL_Event _inputDownEvent; // stores the last input event on this frame that is relevant for input config
  SDL_Event _prevInputDownEvent;
  std::vector<SDL_Joystick *> _joysticks;
  // Key state with the debug console's injected keys merged in; only used in
  // --debug-stdin mode, where _keys points into this (SDL's own array is
  // read-only). See AGENTS.md.
  std::vector<uint8_t> _debugKeys;

  void filterInputDownEvent(const SDL_Event &e);
  SDL_JoystickID _joyIdWithinDirection;
  Uint8 _joyAxisWithinDirection;
};

#endif /* INPUTMANAGER_H_ */
